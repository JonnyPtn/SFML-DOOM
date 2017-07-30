/*

TiMidity -- Experimental MIDI to WAVE converter
Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

instrum.c

Code to load and unload GUS-compatible instrument patches.

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <cstring>

#include <timidity/config.h>
#include <timidity/common.h>
#include <timidity/instrum.h>
#include <timidity/playmidi.h>
#include <timidity/output.h>
#include <timidity/resample.h>
#include <tables.hpp>
#include <timidity/filter.h>



//void Real_Tim_Free( void *pt );

/* Some functions get aggravated if not even the standard banks are
available. */
static ToneBank standard_tonebank, standard_drumset;
ToneBank
*tonebank[128] = { &standard_tonebank },
*drumset[128] = { &standard_drumset };

/* This is a special instrument, used for all melodic programs */
Instrument *default_instrument = 0;

/* This is only used for tracks that don't specify a program */
int default_program = DEFAULT_PROGRAM;

int antialiasing_allowed = 0;
#ifdef FAST_DECAY
int fast_decay = 1;
#else
int fast_decay = 0;
#endif

static void free_instrument(Instrument *ip)
{
    Sample *sp;
    int i;
    if (!ip) return;
    for (i = 0; i<ip->samples; i++)
    {
        sp = &(ip->sample[i]);
        Real_Tim_Free(sp->data);
    }
    Real_Tim_Free(ip->sample);
    Real_Tim_Free(ip);
}

static void free_bank(int dr, int b)
{
    int i;
    ToneBank *bank = ((dr) ? drumset[b] : tonebank[b]);
    for (i = 0; i<128; i++) {
        if (bank->tone[i].instrument)
        {
            /* Not that this could ever happen, of course */
            if (bank->tone[i].instrument != MAGIC_LOAD_INSTRUMENT)
                free_instrument(bank->tone[i].instrument);
            bank->tone[i].instrument = 0;
        }
        if (bank->tone[i].name)
        {
            Real_Tim_Free(bank->tone[i].name);
            bank->tone[i].name = NULL;
        }
    }
}

static  int32_t convert_envelope_rate(uint8_t rate)
{
    int32_t r;

    r = 3 - ((rate >> 6) & 0x3);
    r *= 3;
    r = (int32_t)(rate & 0x3f) << r; /* 6.9 fixed point */

                                     /* 15.15 fixed point. */
    return (((r * 44100) / play_mode->rate) * control_ratio)
        << ((fast_decay) ? 10 : 9);
}

static  int32_t  convert_envelope_offset(uint8_t offset)
{
    /* This is not too good... Can anyone tell me what these values mean?
    Are they GUS-style "exponential" volumes? And what does that mean? */

    /* 15.15 fixed point */
    return offset << (7 + 15);
}

static  int32_t convert_tremolo_sweep(uint8_t sweep)
{
    if (!sweep)
        return 0;

    return
        ((control_ratio * SWEEP_TUNING) << SWEEP_SHIFT) /
        (play_mode->rate * sweep);
}

static  int32_t convert_vibrato_sweep(uint8_t sweep, int32_t vib_control_ratio)
{
    if (!sweep)
        return 0;

    return
        (int32_t)(FSCALE((double)(vib_control_ratio)* SWEEP_TUNING, SWEEP_SHIFT)
            / (double)(play_mode->rate * sweep));

    /* this was overflowing with seashore.pat

    ((vib_control_ratio * SWEEP_TUNING) << SWEEP_SHIFT) /
    (play_mode->rate * sweep); */
}

static  int32_t convert_tremolo_rate(uint8_t rate)
{
    return
        ((SINE_CYCLE_LENGTH * control_ratio * rate) << RATE_SHIFT) /
        (TREMOLO_RATE_TUNING * play_mode->rate);
}

static  int32_t convert_vibrato_rate(uint8_t rate)
{
    /* Return a suitable vibrato_control_ratio value */
    return
        (VIBRATO_RATE_TUNING * play_mode->rate) /
        (rate * 2 * VIBRATO_SAMPLE_INCREMENTS);
}

static void reverse_data(int16_t *sp, int32_t ls, int32_t le)
{
    int16_t s, *ep = sp + le;
    sp += ls;
    le -= ls;
    le /= 2;
    while (le--)
    {
        s = *sp;
        *sp++ = *ep;
        *ep-- = s;
    }
}

/*
If panning or note_to_use != -1, it will be used for all samples,
instead of the sample-specific values in the instrument file.

For note_to_use, any value <0 or >127 will be forced to 0.

For other parameters, 1 means yes, 0 means no, other values are
undefined.

TODO: do reverse loops right */
static Instrument *load_instrument(char *name, int percussion,
    int panning, int amp, int note_to_use,
    int strip_loop, int strip_envelope,
    int strip_tail)
{
    Instrument *ip;
    Sample *sp;
    std::ifstream * fp;
    int i, j, noluck = 0;
    char *path;
    char filename[1024];
#ifdef PATCH_EXT_LIST
    static char *patch_ext[] = PATCH_EXT_LIST;
#endif

    if (!name) 
        return 0;

    path = "eawpats/";

    std::string instName = name;
    //instName.ToUpper();

    strcpy(filename, path);
    strcat(filename, instName.c_str());
    strcat(filename, ".PAT");

    /* Open patch file */
    if ((fp = open_file(filename, 1, 0)) == NULL)
    {
        noluck = 1;
#ifdef PATCH_EXT_LIST
        /* Try with various extensions */
        for (i = 0; patch_ext[i]; i++)
        {
            if (strlen(name) + strlen(patch_ext[i])<1024)
            {
                strcpy(filename, path);
                strcat(filename, name);
                strcat(filename, patch_ext[i]);
                if ((fp = open_file(filename, 1, 0)) != NULL)
                {
                    noluck = 0;
                    break;
                }
            }
        }
#endif
    }

    if (noluck)
    {
        printf(
            "Instrument `%s' can't be found.\n", name);
        return 0;
    }

    printf("Loading instrument %s\n", current_filename.c_str());

    /* Read some headers and do cursory sanity checks. There are loads
    of magic offsets. This could be rewritten... */

    //just load the whole damn file
    std::vector<char> file;
    char c;
    fp->seekg(std::ios::end);
    fp->seekg(std::ios::beg);
    while (fp->good())
    {
        fp->get(c);
        file.push_back(c);
    }
    auto patchID = file.data();
    if (memcmp(patchID, "GF1PATCH110\0ID#000002", 22) &&
        memcmp(patchID, "GF1PATCH100\0ID#000002", 22)) /* don't know what the
                                                        differences are */
    {
        printf("%s: not an instrument\n", name);
        return 0;
    }

    if (file[82] != 1 && file[82] != 0) /* instruments. To some patch makers,
                                      0 means 1 */
    {
        printf(
            "Can't handle patches with %d instruments\n", file[82]);
        return 0;
    }

    if (file[151] != 1 && file[151] != 0) /* layers. What's a layer? */
    {
        printf(
            "Can't handle instruments with %d layers\n", file[151]);
        return 0;
    }

    ip = (Instrument *)safe_malloc(sizeof(Instrument));
    ip->samples =  file[198];
    ip->sample = (Sample *)safe_malloc(sizeof(Sample) * ip->samples);
    std::memset(ip->sample, 0, sizeof(Sample) * ip->samples);

    int mi = 239;
    for (i = 0; i < ip->samples; i++)
    {

        uint8_t fractions;
        int32_t tmplong;
        uint16_t tmpshort;

        auto  READ_CHAR = [&](uint8_t& thing)
        {
            thing = file[mi++];
        };
        auto READ_SHORT = [&](uint16_t& thing)
        {
            std::memcpy(&tmpshort, &file[mi], 2);
            mi += 2;
            thing = LE_SHORT(tmpshort);
        };
        auto READ_LONG = [&](int32_t& thing)
        {
            std::memcpy(&tmplong, &file[mi],4); 
            mi += 4;
            thing = LE_LONG(tmplong);
        };

        mi += 7; /* Skip the wave name */

        fractions = file[mi++];

        sp = &(ip->sample[i]);

        READ_LONG(sp->data_length);
        READ_LONG(sp->loop_start);
        READ_LONG(sp->loop_end);
        uint16_t temp;
        READ_SHORT(temp);
        sp->sample_rate = temp;
        READ_LONG(sp->low_freq);
        READ_LONG(sp->high_freq);
        READ_LONG(sp->root_freq);
        mi += 2; /* Why have a "root frequency" and then "tuning"?? */

        uint8_t pan;
        READ_CHAR(pan);

        if (panning == -1)
            sp->panning = (pan * 8 + 4) & 0x7f;
        else
            sp->panning = (uint8_t)(panning & 0x7F);

        /* envelope, tremolo, and vibrato */

        if (!file[mi+13] || !file[mi+14])
        {
            sp->tremolo_sweep_increment =
                sp->tremolo_phase_increment = sp->tremolo_depth = 0;
            printf( " * no tremolo\n");
        }
        else
        {
            sp->tremolo_sweep_increment = convert_tremolo_sweep(file[mi + 12]);
            sp->tremolo_phase_increment = convert_tremolo_rate(file[mi + 13]);
            sp->tremolo_depth = file[mi + 14];
            printf(
                " * tremolo: sweep %d, phase %d, depth %d\n",
                sp->tremolo_sweep_increment, sp->tremolo_phase_increment,
                sp->tremolo_depth);
        }

        if (!file[mi + 16] || !file[mi + 17])
        {
            sp->vibrato_sweep_increment =
                sp->vibrato_control_ratio = sp->vibrato_depth = 0;
            printf( " * no vibrato\n");
        }
        else
        {
            sp->vibrato_control_ratio = convert_vibrato_rate(file[mi + 16]);
            sp->vibrato_sweep_increment =
                convert_vibrato_sweep(file[mi + 15], sp->vibrato_control_ratio);
            sp->vibrato_depth = file[mi + 17];
            printf(
                " * vibrato: sweep %d, ctl %d, depth %d\n",
                sp->vibrato_sweep_increment, sp->vibrato_control_ratio,
                sp->vibrato_depth);

        }
        mi += 18; //last block was 18 bytes big
        READ_CHAR(sp->modes);
        mi -= 18; //for correct file index still

                      /* Mark this as a fixed-pitch instrument if such a deed is desired. */
        if (note_to_use != -1)
            sp->note_to_use = (uint8_t)(note_to_use);
        else
            sp->note_to_use = 0;

        /* seashore.pat in the Midia patch set has no Sustain. I don't
        understand why, and fixing it by adding the Sustain flag to
        all looped patches probably breaks something else. We do it
        anyway. */

        if (sp->modes & MODES_LOOPING)
            sp->modes |= MODES_SUSTAIN;

        /* Strip any loops and envelopes we're permitted to */
        if ((strip_loop == 1) &&
            (sp->modes & (MODES_SUSTAIN | MODES_LOOPING |
                MODES_PINGPONG | MODES_REVERSE)))
        {
            printf( " - Removing loop and/or sustain\n");
            sp->modes &= ~(MODES_SUSTAIN | MODES_LOOPING |
                MODES_PINGPONG | MODES_REVERSE);
        }

        if (strip_envelope == 1)
        {
            if (sp->modes & MODES_ENVELOPE)
                printf(" - Removing envelope\n");
            sp->modes &= ~MODES_ENVELOPE;
        }
        else if (strip_envelope != 0)
        {
            /* Have to make a guess. */
            if (!(sp->modes & (MODES_LOOPING | MODES_PINGPONG | MODES_REVERSE)))
            {
                /* No loop? Then what's there to sustain? No envelope needed
                either... */
                sp->modes &= ~(MODES_SUSTAIN | MODES_ENVELOPE);
                printf(
                    " - No loop, removing sustain and envelope\n");
            }
            else if (!memcmp(&file[mi], "??????", 6) || file[mi+11] >= 100)
            {
                /* Envelope rates all maxed out? Envelope end at a high "offset"?
                That's a weird envelope. Take it out. */
                sp->modes &= ~MODES_ENVELOPE;
                printf(
                    " - Weirdness, removing envelope\n");
            }
            else if (!(sp->modes & MODES_SUSTAIN))
            {
                /* No sustain? Then no envelope.  I don't know if this is
                justified, but patches without sustain usually don't need the
                envelope either... at least the Gravis ones. They're mostly
                drums.  I think. */
                sp->modes &= ~MODES_ENVELOPE;
                printf(
                    " - No sustain, removing envelope\n");
            }
        }

        for (j = 0; j<6; j++)
        {
            sp->envelope_rate[j] =
                convert_envelope_rate(file[mi + j]);
            sp->envelope_offset[j] =
                convert_envelope_offset(file[mi + 6 + j]);
        }

        mi += 40; /* skip the useless scale frequency, scale factor
                  (what's it mean?), and reserved space */
        mi += 18; //last block was 18 bytes big

        /* Then read the sample data */
        sp->data = (sample_t*)safe_malloc(sp->data_length);
        std::memcpy(sp->data,&file[mi], sp->data_length);
        mi += sp->data_length;

        if (!(sp->modes & MODES_16BIT)) /* convert to 16-bit data */
        {
            int32_t i = sp->data_length;
            uint8_t *cp = (uint8_t *)(sp->data);
            uint16_t *tmp, *anew;
            tmp = anew = (uint16_t*)safe_malloc(sp->data_length * 2);
            while (i--)
                *tmp++ = (uint16_t)(*cp++) << 8;
            cp = (uint8_t *)(sp->data);
            sp->data = (sample_t *)anew;
            Real_Tim_Free(cp);
            sp->data_length *= 2;
            sp->loop_start *= 2;
            sp->loop_end *= 2;
        }
#ifndef LITTLE_ENDIAN
        else
            /* convert to machine byte order */
        {
            int32_t i = sp->data_length / 2;
            int16_t *tmp = (int16_t *)sp->data, s;
            while (i--)
            {
                s = LE_SHORT(*tmp);
                *tmp++ = s;
            }
        }
#endif

        if (sp->modes & MODES_UNSIGNED) /* convert to signed data */
        {
            int32_t i = sp->data_length / 2;
            int16_t *tmp = (int16_t *)sp->data;
            while (i--)
                *tmp++ ^= 0x8000;
        }

        /* Reverse reverse loops and pass them off as normal loops */
        if (sp->modes & MODES_REVERSE)
        {
            int32_t t;
            /* The GUS apparently plays reverse loops by reversing the
            whole sample. We do the same because the GUS does not SUCK. */

            printf("Reverse loop in %s\n", name);
            reverse_data((int16_t *)sp->data, 0, sp->data_length / 2);

            t = sp->loop_start;
            sp->loop_start = sp->data_length - sp->loop_end;
            sp->loop_end = sp->data_length - t;

            sp->modes &= ~MODES_REVERSE;
            sp->modes |= MODES_LOOPING; /* just in case */
        }

        /* If necessary do some anti-aliasing filtering  */

        if (antialiasing_allowed)
            antialiasing(sp, play_mode->rate);

#ifdef ADJUST_SAMPLE_VOLUMES
        if (amp != -1)
            sp->volume = (float)((amp) / 100.0);
        else
        {
            /* Try to determine a volume scaling factor for the sample.
            This is a very crude adjustment, but things sound more
            balanced with it. Still, this should be a runtime option. */
            int32_t i = sp->data_length / 2;
            int16_t maxamp = 0, a;
            int16_t *tmp = (int16_t *)sp->data;
            while (i--)
            {
                a = *tmp++;
                if (a<0) a = -a;
                if (a>maxamp)
                    maxamp = a;
            }
            sp->volume = (float)(32768.0 / maxamp);
            printf(" * volume comp: %f\n", sp->volume);
        }
#else
        if (amp != -1)
            sp->volume = (double)(amp) / 100.0;
        else
            sp->volume = 1.0;
#endif

        sp->data_length /= 2; /* These are in bytes. Convert into samples. */
        sp->loop_start /= 2;
        sp->loop_end /= 2;

        /* Then fractional samples */
        sp->data_length <<= FRACTION_BITS;
        sp->loop_start <<= FRACTION_BITS;
        sp->loop_end <<= FRACTION_BITS;

        /* Adjust for fractional loop points. This is a guess. Does anyone
        know what "fractions" really stands for? */
        sp->loop_start |=
            (fractions & 0x0F) << (FRACTION_BITS - 4);
        sp->loop_end |=
            ((fractions >> 4) & 0x0F) << (FRACTION_BITS - 4);

        /* If this instrument will always be played on the same note,
        and it's not looped, we can resample it now. */
        if (sp->note_to_use && !(sp->modes & MODES_LOOPING))
            pre_resample(sp);

#ifdef LOOKUP_HACK
        /* Squash the 16-bit data into 8 bits. */
        {
            uint8_t *gulp, *ulp;
            int16_t *swp;
            int l = sp->data_length >> FRACTION_BITS;
            gulp = ulp = safe_malloc(l + 1);
            swp = (int16_t *)sp->data;
            while (l--)
                *ulp++ = (*swp++ >> 8) & 0xFF;
            Real_Tim_Free(sp->data);
            sp->data = (sample_t *)gulp;
        }
#endif

        if (strip_tail == 1)
        {
            /* Let's not really, just say we did. */
            printf(" - Stripping tail\n");
            sp->data_length = sp->loop_end;
        }
    }

    delete fp;

    return ip;
}

static int fill_bank(int dr, int b)
{
    int i, errors = 0;
    ToneBank *bank = ((dr) ? drumset[b] : tonebank[b]);
    if (!bank)
    {
        printf(
            "Huh. Tried to load instruments in non-existent %s %d\n",
            (dr) ? "drumset" : "tone bank", b);
        return 0;
    }
    for (i = 0; i<128; i++)
    {
        if (bank->tone[i].instrument == MAGIC_LOAD_INSTRUMENT)
        {
            if (!(bank->tone[i].name))
            {
                printf(
                    "No instrument mapped to %s %d, program %d%s",
                    (dr) ? "drum set" : "tone bank", b, i,
                    (b != 0) ? "" : " - this instrument will not be heard\n");
                if (b != 0)
                {
                    /* Mark the corresponding instrument in the default
                    bank / drumset for loading (if it isn't already) */
                    if (!dr)
                    {
                        if (!(standard_tonebank.tone[i].instrument))
                            standard_tonebank.tone[i].instrument =
                            MAGIC_LOAD_INSTRUMENT;
                    }
                    else
                    {
                        if (!(standard_drumset.tone[i].instrument))
                            standard_drumset.tone[i].instrument =
                            MAGIC_LOAD_INSTRUMENT;
                    }
                }
                bank->tone[i].instrument = 0;
                errors++;
            }
            else if (!(bank->tone[i].instrument =
                load_instrument(bank->tone[i].name,
                (dr) ? 1 : 0,
                    bank->tone[i].pan,
                    bank->tone[i].amp,
                    (bank->tone[i].note != -1) ?
                    bank->tone[i].note :
                    ((dr) ? i : -1),
                    (bank->tone[i].strip_loop != -1) ?
                    bank->tone[i].strip_loop :
                    ((dr) ? 1 : -1),
                    (bank->tone[i].strip_envelope != -1) ?
                    bank->tone[i].strip_envelope :
                    ((dr) ? 1 : -1),
                    bank->tone[i].strip_tail)))
            {
                printf(
                    "Couldn't load instrument %s (%s %d, program %d)\n",
                    bank->tone[i].name,
                    (dr) ? "drum set" : "tone bank", b, i);
                errors++;
            }
        }
    }
    return errors;
}

int load_missing_instruments(void)
{
    int i = 128, errors = 0;
    while (i--)
    {
        if (tonebank[i])
            errors += fill_bank(0, i);
        if (drumset[i])
            errors += fill_bank(1, i);
    }
    return errors;
}

void free_instruments(void)
{
    int i = 128;
    while (i--)
    {
        if (tonebank[i])
            free_bank(0, i);
        if (drumset[i])
            free_bank(1, i);
    }
}

int set_default_instrument(char *name)
{
    Instrument *ip;
    if (!(ip = load_instrument(name, 0, -1, -1, -1, 0, 0, 0)))
        return -1;
    if (default_instrument)
        free_instrument(default_instrument);
    default_instrument = ip;
    default_program = SPECIAL_PROGRAM;
    return 0;
}
