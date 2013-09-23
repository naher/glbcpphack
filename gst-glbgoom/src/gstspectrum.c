/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 *               <2006> Stefan Kost <ensonic@users.sf.net>
 *               <2007> Sebastian Dröge <slomo@circular-chaos.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/**
 * SECTION:element-spectrum
 * @short_description: audio spectrum analyzer
 *
 * <refsect2>
 * <para>
 * The Spectrum element analyzes the frequency spectrum of an audio signal.
 * If the #GstSpectrum:message property is #TRUE, it sends analysis results as
 * application messages named
 * <classname>&quot;spectrum&quot;</classname> after each interval of time given
 * by the #GstSpectrum:interval property.
 * </para>
 * <para>
 * The message's structure contains some combination of these fields:
 * <itemizedlist>
 * <listitem>
 *   <para>
 *   #GstClockTime
 *   <classname>&quot;endtime&quot;</classname>:
 *   the end time of the buffer that triggered the message. Always present.
 *   </para>
 * </listitem>
 * <listitem>
 *   <para>
 *   #GstValueList of #gfloat
 *   <classname>&quot;magnitude&quot;</classname>:
 *   the level for each frequency band in dB. All values below the value of the
 *   #GstSpectrum:threshold property will be set to the threshold. Only present
 *   if the message-magnitude property is true.
 *   </para>
 * </listitem>
 * <listitem>
 *   <para>
 *   #GstValueList of #gfloat
 *   <classname>&quot;phase&quot;</classname>:
 *   The phase for each frequency band. The value is between -pi and pi. Only
 *   present if the message-phase property is true.
 *   </para>
 * </listitem>
 * </itemizedlist>
 * </para>
 * <para>
 * This element cannot be used with the gst-launch command in a sensible way.
 * This sample code demonstrates how to use it in an application.
 * </para>
 * <title>Example application</title>
 * <para>
 * <include xmlns="http://www.w3.org/2003/XInclude" href="element-spectrum-example.xml" />
 * </para>
 * <para>
 * Last reviewed on 2008-02-09 (0.10.6)
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <gst/audio/audio.h>
#include <gst/audio/gstaudiofilter.h>
#include <math.h>
#include "gstspectrum.h"

#include <gst/fft/gstfft.h>
#include <gst/fft/gstffts16.h>
#include <gst/fft/gstffts32.h>
#include <gst/fft/gstfftf32.h>
#include <gst/fft/gstfftf64.h>

GST_DEBUG_CATEGORY_STATIC (gst_spectrum_debug);
#define GST_CAT_DEFAULT gst_spectrum_debug

/* elementfactory information */
static const GstElementDetails gst_spectrum_details =
GST_ELEMENT_DETAILS ("Spectrum analyzer",
    "Filter/Analyzer/Audio",
    "Run an FFT on the audio signal, output spectrum data",
    "Erik Walthinsen <omega@cse.ogi.edu>, "
    "Stefan Kost <ensonic@users.sf.net>, "
    "Sebastian Dröge <slomo@circular-chaos.org>");

#define ALLOWED_CAPS \
    "audio/x-raw-int, "                                               \
    " width = (int) 16, "                                             \
    " depth = (int) 16, "                                             \
    " signed = (boolean) true, "                                      \
    " endianness = (int) BYTE_ORDER, "                                \
    " rate = (int) [ 1, MAX ], "                                      \
    " channels = (int) [ 1, MAX ]; "                                  \
    "audio/x-raw-int, "                                               \
    " width = (int) 32, "                                             \
    " depth = (int) 32, "                                             \
    " signed = (boolean) true, "                                      \
    " endianness = (int) BYTE_ORDER, "                                \
    " rate = (int) [ 1, MAX ], "                                      \
    " channels = (int) [ 1, MAX ]; "                                  \
    "audio/x-raw-float, "                                             \
    " width = (int) { 32, 64 }, "                                     \
    " endianness = (int) BYTE_ORDER, "                                \
    " rate = (int) [ 1, MAX ], "                                      \
    " channels = (int) [ 1, MAX ]"

/* Spectrum properties */
#define DEFAULT_SIGNAL_SPECTRUM		TRUE
#define DEFAULT_SIGNAL_MAGNITUDE	TRUE
#define DEFAULT_SIGNAL_PHASE		FALSE
#define DEFAULT_SIGNAL_INTERVAL		(GST_SECOND / 10)
#define DEFAULT_BANDS			128
#define DEFAULT_THRESHOLD		-60

#define SPECTRUM_WINDOW_BASE 9
#define SPECTRUM_WINDOW_LEN (1 << (SPECTRUM_WINDOW_BASE+1))

enum
{
  PROP_0,
  PROP_SIGNAL_SPECTRUM,
  PROP_SIGNAL_MAGNITUDE,
  PROP_SIGNAL_PHASE,
  PROP_SIGNAL_INTERVAL,
  PROP_BANDS,
  PROP_THRESHOLD
};

//GST_BOILERPLATE (GstSpectrum, gst_spectrum, GstAudioFilter,
//    GST_TYPE_AUDIO_FILTER);

//static void gst_spectrum_dispose (GObject * object);
//static void gst_spectrum_finalize (GObject * object);
//static void gst_spectrum_set_property (GObject * object, guint prop_id,
//    const GValue * value, GParamSpec * pspec);
//static void gst_spectrum_get_property (GObject * object, guint prop_id,
//    GValue * value, GParamSpec * pspec);
//static gboolean gst_spectrum_start (GstBaseTransform * trans);
//static gboolean gst_spectrum_stop (GstBaseTransform * trans);
//static gboolean gst_spectrum_event (GstBaseTransform * trans, GstEvent * event);
//static GstFlowReturn gst_spectrum_transform_ip (GstBaseTransform * trans,
static GstFlowReturn gst_spectrum_transform_ip (GstSpectrum * trans,
    GstBuffer * in);
//static gboolean gst_spectrum_setup (GstAudioFilter * base,
//    GstRingBufferSpec * format);

static void process_s16 (GstSpectrum * spectrum, const gint16 * samples);
static void process_s32 (GstSpectrum * spectrum, const gint32 * samples);
static void process_f32 (GstSpectrum * spectrum, const gfloat * samples);
static void process_f64 (GstSpectrum * spectrum, const gdouble * samples);

static void spectrum_init (GstSpectrum * spectrum) {
  // gst_spectrum_init
  spectrum->message = DEFAULT_SIGNAL_SPECTRUM;
  spectrum->message_magnitude = DEFAULT_SIGNAL_MAGNITUDE;
  spectrum->message_phase = DEFAULT_SIGNAL_PHASE;
  spectrum->interval = DEFAULT_SIGNAL_INTERVAL;
  spectrum->bands = DEFAULT_BANDS;
  spectrum->threshold = DEFAULT_THRESHOLD;
  
  // gst_spectrum_start
  spectrum->num_frames = 0;
  spectrum->num_fft = 0;
  spectrum->spect_magnitude = g_new0 (gfloat, spectrum->bands);
  spectrum->spect_phase = g_new0 (gfloat, spectrum->bands);

  // gst_spectrum_setup
  spectrum->in = NULL;
  spectrum->fft_ctx = NULL;
  spectrum->fft_free_func = NULL;
  spectrum->freqdata = NULL;
  spectrum->process = (GstSpectrumProcessFunc) process_s32;
}
//
//static gboolean
//gst_spectrum_start (GstBaseTransform * trans)
//{
//  GstSpectrum *filter = GST_SPECTRUM (trans);
//
//  filter->num_frames = 0;
//  filter->num_fft = 0;
//  if (filter->spect_magnitude)
//    memset (filter->spect_magnitude, 0, filter->bands * sizeof (gfloat));
//  if (filter->spect_phase)
//    memset (filter->spect_phase, 0, filter->bands * sizeof (gfloat));
//
//  return TRUE;
//}
//
//static gboolean
//gst_spectrum_stop (GstBaseTransform * trans)
//{
//  GstSpectrum *filter = GST_SPECTRUM (trans);
//
//  gst_adapter_clear (filter->adapter);
//
//  return TRUE;
//}
//
//static gboolean
//gst_spectrum_event (GstBaseTransform * trans, GstEvent * event)
//{
//  GstSpectrum *filter = GST_SPECTRUM (trans);
//
//  switch (GST_EVENT_TYPE (event)) {
//    case GST_EVENT_FLUSH_STOP:
//    case GST_EVENT_EOS:
//      gst_adapter_clear (filter->adapter);
//      break;
//    default:
//      break;
//  }
//
//  return TRUE;
//}
//
//static gboolean
//gst_spectrum_setup (GstAudioFilter * base, GstRingBufferSpec * format)
//{
//  GstSpectrum *filter = GST_SPECTRUM (base);
//
//  if (filter->in) {
//    g_free (filter->in);
//    filter->in = NULL;
//  }
//
//  if (filter->fft_free_func) {
//    filter->fft_free_func (filter->fft_ctx);
//    filter->fft_ctx = NULL;
//    filter->fft_free_func = NULL;
//  }
//
//  if (filter->freqdata) {
//    g_free (filter->freqdata);
//    filter->freqdata = NULL;
//  }
//
//  if (format->type == GST_BUFTYPE_LINEAR && format->width == 32)
//    filter->process = (GstSpectrumProcessFunc) process_s32;
//  else if (format->type == GST_BUFTYPE_LINEAR && format->width == 16)
//    filter->process = (GstSpectrumProcessFunc) process_s16;
//  else if (format->type == GST_BUFTYPE_FLOAT && format->width == 64)
//    filter->process = (GstSpectrumProcessFunc) process_f64;
//  else if (format->type == GST_BUFTYPE_FLOAT && format->width == 32)
//    filter->process = (GstSpectrumProcessFunc) process_f32;
//  else
//    g_assert_not_reached ();
//
//  return TRUE;
//}

static GstMessage *
gst_spectrum_message_new (GstSpectrum * spectrum, GstClockTime endtime)
{
  GstStructure *s;
  GValue v = { 0, };
  GValue *l;
  guint i;
  gfloat *spect_magnitude = spectrum->spect_magnitude;
  gfloat *spect_phase = spectrum->spect_phase;

  GST_DEBUG_OBJECT (spectrum, "preparing message, spect = %p, bands =%d ",
      spect_magnitude, spectrum->bands);

  s = gst_structure_new ("spectrum", "endtime", GST_TYPE_CLOCK_TIME,
      endtime, NULL);

  if (spectrum->message_magnitude) {
    g_value_init (&v, GST_TYPE_LIST);
    /* will copy-by-value */
    gst_structure_set_value (s, "magnitude", &v);
    g_value_unset (&v);

    g_value_init (&v, G_TYPE_FLOAT);
    l = (GValue *) gst_structure_get_value (s, "magnitude");
    for (i = 0; i < spectrum->bands; i++) {
      g_value_set_float (&v, spect_magnitude[i]);
      gst_value_list_append_value (l, &v);      /* copies by value */
    }
    g_value_unset (&v);
  }

  if (spectrum->message_phase) {
    g_value_init (&v, GST_TYPE_LIST);
    /* will copy-by-value */
    gst_structure_set_value (s, "phase", &v);
    g_value_unset (&v);

    g_value_init (&v, G_TYPE_FLOAT);
    l = (GValue *) gst_structure_get_value (s, "phase");
    for (i = 0; i < spectrum->bands; i++) {
      g_value_set_float (&v, spect_phase[i]);
      gst_value_list_append_value (l, &v);      /* copies by value */
    }
    g_value_unset (&v);
  }

  return gst_message_new_element (GST_OBJECT (spectrum), s);
}

#define DEFINE_PROCESS_FUNC_INT(width,next_width,max) \
static void \
process_s##width (GstSpectrum *spectrum, const gint##width *samples) \
{ \
  gfloat *spect_magnitude = spectrum->spect_magnitude; \
  gfloat *spect_phase = spectrum->spect_phase; \
  gint channels = /*GST_AUDIO_FILTER (spectrum)->format.channels*/ 1; \
  gint i, j, k; \
  gint##next_width acc; \
  GstFFTS##width##Complex *freqdata; \
  GstFFTS##width *ctx; \
  gint##width *in; \
  gint nfft = 2 * spectrum->bands - 2; \
  \
  if (!spectrum->in) \
    spectrum->in = (guint8 *) g_new (gint##width, nfft); \
  \
  in = (gint##width *) spectrum->in; \
  \
  for (i = 0, j = 0; i < nfft; i++) { \
    /* convert to mono */ \
    for (k = 0, acc = 0; k < channels; k++) \
      acc += samples[j++]; \
    in[i] = (gint##width) (acc / channels); \
  } \
  \
  if (!spectrum->fft_ctx) { \
    spectrum->fft_ctx = gst_fft_s##width##_new (nfft, FALSE); \
    spectrum->fft_free_func = (GstSpectrumFFTFreeFunc) gst_fft_s##width##_free; \
  } \
  ctx = spectrum->fft_ctx; \
  \
  gst_fft_s##width##_window (ctx, in, GST_FFT_WINDOW_HAMMING); \
  \
  if (!spectrum->freqdata) \
    spectrum->freqdata = g_new (GstFFTS##width##Complex, spectrum->bands); \
  \
  freqdata = (GstFFTS##width##Complex *) spectrum->freqdata; \
  \
  gst_fft_s##width##_fft (ctx, in, freqdata); \
  spectrum->num_fft++; \
  \
  /* Calculate magnitude in db */ \
  for (i = 0; i < spectrum->bands; i++) { \
    gdouble val = 0.0; \
    val = (gdouble) freqdata[i].r * (gdouble) freqdata[i].r; \
    val += (gdouble) freqdata[i].i * (gdouble) freqdata[i].i; \
    val /= max*max; \
    val = 10.0 * log10 (val); \
    if (val < spectrum->threshold) \
      val = spectrum->threshold; \
    spect_magnitude[i] += val; \
  } \
   \
  /* Calculate phase */ \
  for (i = 0; i < spectrum->bands; i++) \
    spect_phase[i] += atan2 (freqdata[i].i, freqdata[i].r); \
   \
}

DEFINE_PROCESS_FUNC_INT (16, 32, 32767.0);
DEFINE_PROCESS_FUNC_INT (32, 64, 2147483647.0);

#define DEFINE_PROCESS_FUNC_FLOAT(width,type) \
static void \
process_f##width (GstSpectrum *spectrum, const g##type *samples) \
{ \
  gfloat *spect_magnitude = spectrum->spect_magnitude; \
  gfloat *spect_phase = spectrum->spect_phase; \
  gint channels = 1/*GST_AUDIO_FILTER (spectrum)->format.channels*/; \
  gint i, j, k; \
  g##type acc; \
  GstFFTF##width##Complex *freqdata; \
  GstFFTF##width *ctx; \
  g##type *in; \
  gint nfft = 2 * spectrum->bands - 2; \
  \
  if (!spectrum->in) \
    spectrum->in = (guint8 *) g_new (g##type, nfft); \
  \
  in = (g##type *) spectrum->in; \
  \
  for (i = 0, j = 0; i < nfft; i++) { \
    /* convert to mono */ \
    for (k = 0, acc = 0; k < channels; k++) \
      acc += samples[j++]; \
    in[i] = (g##type) (acc / channels); \
    if (abs (in[i]) > 1.0) \
      g_assert_not_reached(); \
  } \
  \
  if (!spectrum->fft_ctx) { \
    spectrum->fft_ctx = gst_fft_f##width##_new (nfft, FALSE); \
    spectrum->fft_free_func = (GstSpectrumFFTFreeFunc) gst_fft_f##width##_free; \
  } \
  ctx = spectrum->fft_ctx; \
  \
  gst_fft_f##width##_window (ctx, in, GST_FFT_WINDOW_HAMMING); \
  \
  if (!spectrum->freqdata) \
    spectrum->freqdata = g_new (GstFFTF##width##Complex, spectrum->bands); \
  \
  freqdata = (GstFFTF##width##Complex *) spectrum->freqdata; \
  \
  gst_fft_f##width##_fft (ctx, in, freqdata); \
  spectrum->num_fft++; \
  \
  /* Calculate magnitude in db */ \
  for (i = 0; i < spectrum->bands; i++) { \
    gdouble val = 0.0; \
    val = freqdata[i].r * freqdata[i].r; \
    val += freqdata[i].i * freqdata[i].i; \
    val /= nfft*nfft; \
    val = 10.0 * log10 (val); \
    if (val < spectrum->threshold) \
      val = spectrum->threshold; \
    spect_magnitude[i] += val; \
  } \
   \
  /* Calculate phase */ \
  for (i = 0; i < spectrum->bands; i++) \
    spect_phase[i] += atan2 (freqdata[i].i, freqdata[i].r); \
   \
}

DEFINE_PROCESS_FUNC_FLOAT (32, float);
DEFINE_PROCESS_FUNC_FLOAT (64, double);

static GstFlowReturn
//gst_spectrum_transform_ip (GstBaseTransform * trans, GstBuffer * in)
gst_spectrum_transform_ip (GstSpectrum * trans, GstBuffer * in)
{
  GstSpectrum *spectrum = trans;
  gint wanted;
  gint i;
  gfloat *spect_magnitude = spectrum->spect_magnitude;
  gfloat *spect_phase = spectrum->spect_phase;
  gint rate = /*GST_AUDIO_FILTER (spectrum)->format.rate*/1;
  gint channels = /*GST_AUDIO_FILTER (spectrum)->format.channels*/1;
  gint width = /*GST_AUDIO_FILTER (spectrum)->format.width*/32 / 8;
  gint nfft = 2 * spectrum->bands - 2;

  /*GstClockTime endtime =
      gst_segment_to_running_time (&trans->segment, GST_FORMAT_TIME,
      GST_BUFFER_TIMESTAMP (in));
  GstClockTime blktime = GST_FRAMES_TO_CLOCK_TIME (nfft, rate);*/

  GST_LOG_OBJECT (spectrum, "input size: %d bytes", GST_BUFFER_SIZE (in));

  /* can we do this nicer? */
  //gst_adapter_push (spectrum->adapter, gst_buffer_copy (in));
  /* required number of bytes */
  wanted = channels * nfft * width;

  while (gst_adapter_available (spectrum->adapter) >= wanted) {
    const guint8 *samples;

    samples = gst_adapter_peek (spectrum->adapter, wanted);

    spectrum->process (spectrum, samples);

    spectrum->num_frames += nfft;
    /*endtime += blktime;*/
    /* do we need to message ? */
    if (spectrum->num_frames >=
        GST_CLOCK_TIME_TO_FRAMES (spectrum->interval, rate)) {
      if (spectrum->message) {
        GstMessage *m;

        /* Calculate average */
        for (i = 0; i < spectrum->bands; i++) {
          spect_magnitude[i] /= spectrum->num_fft;
          spect_phase[i] /= spectrum->num_fft;
        }

        //m = gst_spectrum_message_new (spectrum, endtime);

        //gst_element_post_message (GST_ELEMENT (spectrum), m);
      }
      memset (spect_magnitude, 0, spectrum->bands * sizeof (gfloat));
      memset (spect_phase, 0, spectrum->bands * sizeof (gfloat));
      spectrum->num_frames = 0;
      spectrum->num_fft = 0;
    }

    gst_adapter_flush (spectrum->adapter, wanted);
  }

  return GST_FLOW_OK;
}
