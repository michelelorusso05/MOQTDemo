/* Error codes -1, -11, -12, and -39 are retired and reserved. */

#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MOQ_ERROR_MOQ -2

#define MOQ_ERROR_URL -3

#define MOQ_ERROR_UTF8 -4

#define MOQ_ERROR_CONNECT -5

#define MOQ_ERROR_INVALID_POINTER -6

#define MOQ_ERROR_INVALID_ID -7

#define MOQ_ERROR_NOT_FOUND -8

#define MOQ_ERROR_UNKNOWN_FORMAT -9

#define MOQ_ERROR_INIT_FAILED -10

#define MOQ_ERROR_TIMESTAMP_OVERFLOW -13

#define MOQ_ERROR_LEVEL -14

#define MOQ_ERROR_INVALID_CODE -15

#define MOQ_ERROR_PANIC -16

#define MOQ_ERROR_OFFLINE -17

#define MOQ_ERROR_HANG -18

#define MOQ_ERROR_NO_INDEX -19

#define MOQ_ERROR_NUL -20

#define MOQ_ERROR_SESSION_NOT_FOUND -21

#define MOQ_ERROR_ORIGIN_NOT_FOUND -22

#define MOQ_ERROR_ANNOUNCEMENT_NOT_FOUND -23

#define MOQ_ERROR_BROADCAST_NOT_FOUND -24

#define MOQ_ERROR_CATALOG_NOT_FOUND -25

#define MOQ_ERROR_MEDIA_NOT_FOUND -26

#define MOQ_ERROR_TRACK_NOT_FOUND -27

#define MOQ_ERROR_FRAME_NOT_FOUND -28

#define MOQ_ERROR_MUX -29

#define MOQ_ERROR_AUDIO -30

#define MOQ_ERROR_BUFFER_NOT_CONSUMED -31

#define MOQ_ERROR_GROUP_NOT_FOUND -32

#define MOQ_ERROR_NATIVE -33

#define MOQ_ERROR_UNAUTHORIZED -34

#define MOQ_ERROR_FORBIDDEN -35

#define MOQ_ERROR_VIDEO -36

#define MOQ_ERROR_JSON -37

#define MOQ_ERROR_JSON_TRACK -38

#define MOQ_ERROR_INVALID_CONFIG -40

#define MOQ_ERROR_UNRESOLVABLE_BROADCAST -41

/**
 * How a media track's frames are wrapped, independent of the codec.
 *
 * The ABI carries this as a `uint32_t`, so an unknown discriminant from C is an
 * error rather than UB.
 */
typedef enum moq_container_kind {
  /**
   * A QUIC VarInt timestamp prefix followed by the raw codec payload.
   * Timestamps are in microseconds.
   */
  MOQ_CONTAINER_KIND_LEGACY = 0,
  /**
   * Fragmented MP4: each frame is a complete moof+mdat fragment, described by
   * the init segment in `moq_container::init`.
   */
  MOQ_CONTAINER_KIND_CMAF = 1,
  /**
   * Low Overhead Container (draft-ietf-moq-loc): a small property block
   * followed by the codec payload.
   */
  MOQ_CONTAINER_KIND_LOC = 2,
  /**
   * A container this build does not recognize, so the rendition must be
   * ignored. Only ever read out of a catalog: publishing it is an error.
   */
  MOQ_CONTAINER_KIND_UNKNOWN = 3,
} moq_container_kind;

/**
 * A single audio codec [moq_publish_audio] can parse.
 */
typedef enum moq_audio_format {
  /**
   * Advanced Audio Coding, configured by an AudioSpecificConfig.
   */
  MOQ_AUDIO_FORMAT_AAC = 0,
  /**
   * Opus, configured by an OpusHead.
   */
  MOQ_AUDIO_FORMAT_OPUS = 1,
  /**
   * FLAC, configured by the `fLaC` marker plus its STREAMINFO block.
   */
  MOQ_AUDIO_FORMAT_FLAC = 2,
  /**
   * MPEG-1/2 Audio Layer III.
   */
  MOQ_AUDIO_FORMAT_MP3 = 3,
} moq_audio_format;

/**
 * Raw PCM sample layout, mirroring WebCodecs `AudioData.format`.
 *
 * The enum is exposed in the C header for readability, but ABI
 * fields/parameters that carry it are typed `u32`. A C caller
 * passing an unknown discriminant gets `Error::InvalidCode` instead
 * of UB.
 *
 * <https://developer.mozilla.org/en-US/docs/Web/API/AudioData/format>
 */
typedef enum moq_audio_sample_format {
  MOQ_AUDIO_SAMPLE_FORMAT_U8 = 0,
  MOQ_AUDIO_SAMPLE_FORMAT_S16 = 1,
  MOQ_AUDIO_SAMPLE_FORMAT_S32 = 2,
  MOQ_AUDIO_SAMPLE_FORMAT_F32 = 3,
  MOQ_AUDIO_SAMPLE_FORMAT_U8_PLANAR = 4,
  MOQ_AUDIO_SAMPLE_FORMAT_S16_PLANAR = 5,
  MOQ_AUDIO_SAMPLE_FORMAT_S32_PLANAR = 6,
  MOQ_AUDIO_SAMPLE_FORMAT_F32_PLANAR = 7,
} moq_audio_sample_format;

/**
 * A single video codec [moq_publish_video] can parse.
 *
 * H.264 and H.265 appear twice each because the framing differs, not just the
 * codec: AVC1/HVC1 are length-prefixed with an out-of-band config record,
 * while AVC3/HEV1 are Annex-B with the parameter sets inline.
 */
typedef enum moq_video_format {
  /**
   * H.264, length-prefixed NALUs with an out-of-band avcC.
   */
  MOQ_VIDEO_FORMAT_AVC1 = 0,
  /**
   * H.264, Annex-B with inline SPS/PPS.
   */
  MOQ_VIDEO_FORMAT_AVC3 = 1,
  /**
   * H.265, length-prefixed NALUs with an out-of-band hvcC.
   */
  MOQ_VIDEO_FORMAT_HVC1 = 2,
  /**
   * H.265, Annex-B with inline parameter sets.
   */
  MOQ_VIDEO_FORMAT_HEV1 = 3,
  /**
   * AV1.
   */
  MOQ_VIDEO_FORMAT_AV01 = 4,
  /**
   * VP8.
   */
  MOQ_VIDEO_FORMAT_VP8 = 5,
  /**
   * VP9.
   */
  MOQ_VIDEO_FORMAT_VP9 = 6,
} moq_video_format;

/**
 * A container [moq_publish_container] can demux, which may publish several tracks.
 */
typedef enum moq_container_format {
  /**
   * Fragmented MP4 / CMAF.
   */
  MOQ_CONTAINER_FORMAT_FMP4 = 0,
  /**
   * Matroska / WebM.
   */
  MOQ_CONTAINER_FORMAT_MKV = 1,
  /**
   * MPEG-2 transport stream.
   */
  MOQ_CONTAINER_FORMAT_TS = 2,
  /**
   * Flash Video, as used by RTMP.
   */
  MOQ_CONTAINER_FORMAT_FLV = 3,
} moq_container_format;

/**
 * Pixel layout of the raw frames handed to [`moq_encode_video_frame`].
 *
 * The enum is exposed in the C header for readability, but ABI fields that
 * carry it are typed `u32`. A C caller passing an unknown discriminant gets
 * `Error::InvalidCode` instead of UB.
 */
typedef enum moq_video_pixel_format {
  /**
   * Tightly-packed planar I420: Y, then U, then V, no row padding.
   * `width * height * 3 / 2` bytes, the same layout [`moq_decode_video`]
   * hands back.
   */
  MOQ_VIDEO_PIXEL_FORMAT_I420 = 0,
  /**
   * Tightly-packed RGBA, `width * height * 4` bytes, no row padding.
   */
  MOQ_VIDEO_PIXEL_FORMAT_RGBA = 1,
} moq_video_pixel_format;

/**
 * Output video codec for [`moq_encode_video`].
 *
 * Not every codec has a backend on every machine: H.265 is hardware-only, so
 * publishing it fails where no hardware encoder is available.
 */
typedef enum moq_video_codec {
  /**
   * H.264 / AVC, published as an `avc3` track.
   */
  MOQ_VIDEO_CODEC_H264 = 0,
  /**
   * H.265 / HEVC, published as a `hev1` track.
   */
  MOQ_VIDEO_CODEC_H265 = 1,
} moq_video_codec;

/**
 * Which encoder implementation [`moq_encode_video`] should use.
 */
typedef enum moq_video_encoder_kind {
  /**
   * Prefer a platform hardware encoder, falling back to software.
   */
  MOQ_VIDEO_ENCODER_KIND_AUTO = 0,
  /**
   * Hardware only; fails if none is available.
   */
  MOQ_VIDEO_ENCODER_KIND_HARDWARE = 1,
  /**
   * Software only (openh264, H.264 only).
   */
  MOQ_VIDEO_ENCODER_KIND_SOFTWARE = 2,
  /**
   * A specific backend, named by `moq_video_encoder_output::encoder`.
   */
  MOQ_VIDEO_ENCODER_KIND_NAMED = 3,
} moq_video_encoder_kind;

/**
 * Whether a protocol code is from the session or stream registry.
 */
typedef enum moq_error_scope {
  /**
   * A session close code.
   */
  MOQ_ERROR_SCOPE_SESSION = 0,
  /**
   * A stream reset or stop code.
   */
  MOQ_ERROR_SCOPE_STREAM = 1,
} moq_error_scope;

/**
 * A recognized protocol kind. Pair with [`moq_error_scope`]: `CANCEL` is 0 on a session
 * and 1 on a stream. `APP` and `UNKNOWN` keep the numeric code in [`moq_protocol_error`].
 */
typedef enum moq_protocol_kind {
  /**
   * Cancel.
   */
  MOQ_PROTOCOL_KIND_CANCEL = 0,
  /**
   * Internal.
   */
  MOQ_PROTOCOL_KIND_INTERNAL = 1,
  /**
   * Unauthorized.
   */
  MOQ_PROTOCOL_KIND_UNAUTHORIZED = 2,
  /**
   * Protocol violation.
   */
  MOQ_PROTOCOL_KIND_PROTOCOL_VIOLATION = 3,
  /**
   * Key value formatting.
   */
  MOQ_PROTOCOL_KIND_KEY_VALUE_FORMATTING = 4,
  /**
   * Goaway timeout.
   */
  MOQ_PROTOCOL_KIND_GOAWAY_TIMEOUT = 5,
  /**
   * Timeout.
   */
  MOQ_PROTOCOL_KIND_TIMEOUT = 6,
  /**
   * Version.
   */
  MOQ_PROTOCOL_KIND_VERSION = 7,
  /**
   * Delivery timeout.
   */
  MOQ_PROTOCOL_KIND_DELIVERY_TIMEOUT = 11,
  /**
   * Session closed.
   */
  MOQ_PROTOCOL_KIND_SESSION_CLOSED = 12,
  /**
   * Going away.
   */
  MOQ_PROTOCOL_KIND_GOING_AWAY = 13,
  /**
   * Too far behind.
   */
  MOQ_PROTOCOL_KIND_TOO_FAR_BEHIND = 14,
  /**
   * Malformed track.
   */
  MOQ_PROTOCOL_KIND_MALFORMED_TRACK = 15,
  /**
   * Not found.
   */
  MOQ_PROTOCOL_KIND_NOT_FOUND = 16,
  /**
   * Unroutable.
   */
  MOQ_PROTOCOL_KIND_UNROUTABLE = 17,
  /**
   * Old.
   */
  MOQ_PROTOCOL_KIND_OLD = 18,
  /**
   * Evicted.
   */
  MOQ_PROTOCOL_KIND_EVICTED = 19,
  /**
   * Wrong size.
   */
  MOQ_PROTOCOL_KIND_WRONG_SIZE = 20,
  /**
   * Frame too large.
   */
  MOQ_PROTOCOL_KIND_FRAME_TOO_LARGE = 21,
  /**
   * Timestamp mismatch.
   */
  MOQ_PROTOCOL_KIND_TIMESTAMP_MISMATCH = 22,
  /**
   * App.
   */
  MOQ_PROTOCOL_KIND_APP = 23,
  /**
   * Unknown.
   */
  MOQ_PROTOCOL_KIND_UNKNOWN = 24,
} moq_protocol_kind;

/**
 * Whether a published track has subscribers, as reported by a demand watcher.
 *
 * The positive values an `on_demand` callback receives; `0` and negative codes are
 * the terminal statuses every callback shares.
 */
typedef enum moq_demand {
  /**
   * At least one subscriber is active.
   */
  MOQ_DEMAND_USED = 1,
  /**
   * No subscriber is active.
   */
  MOQ_DEMAND_UNUSED = 2,
} moq_demand;

/**
 * A protocol failure a peer sent: scope, verbatim wire code, and recognized kind.
 *
 * Filled by [`crate::moq_error_protocol`] after a call returned a negative code. Do not parse
 * [`crate::moq_error`] for this; that string is diagnostics only.
 */
typedef struct moq_protocol_error {
  /**
   * [`moq_error_scope`] discriminant.
   */
  uint32_t scope;
  /**
   * The integer on the wire, kept verbatim.
   */
  uint32_t code;
  /**
   * [`moq_protocol_kind`] discriminant.
   */
  uint32_t kind;
} moq_protocol_error;

/**
 * A borrowed UTF-8 string slice, NOT NULL terminated.
 *
 * Used in both directions. As an output (e.g. a JSON document libmoq hands back) the
 * pointer borrows libmoq's own storage and is only valid until the owning resource is
 * freed; see the function that fills it for the exact lifetime. As an input (e.g. a
 * [moq_client_config] list) the pointer borrows the caller's storage and is only read
 * during the call.
 */
typedef struct moq_string {
  /**
   * Pointer to `len` bytes of UTF-8, NOT NULL terminated.
   */
  const char *data;
  uintptr_t len;
} moq_string;

/**
 * Settings for [moq_session_connect], or NULL to dial with the defaults.
 *
 * Zero it (`memset`, or a `{0}` initializer) and set only what you need: a
 * zeroed struct means the defaults throughout. That is why the knobs whose
 * default is not zero carry a `has_*` flag rather than being read directly. The
 * WebSocket fallback is on by default and the reconnect backoff starts at one
 * second, so a caller who never touched them would otherwise silently turn them
 * off.
 *
 * New settings are appended to the end of this struct, and a zeroed one keeps
 * the previous behavior, so adding one does not disturb existing callers.
 */
typedef struct moq_client_config {
  /**
   * Protocol versions to offer during the handshake, most preferred first.
   * NULL/0 offers everything this build supports. Names are spelled the way
   * the CLI spells them (`moq-lite-05`, `moq-transport-22`); [moq_versions]
   * lists what is on offer.
   */
  const struct moq_string *versions;
  uintptr_t versions_len;
  /**
   * Local socket address to bind, or NULL for the wildcard address.
   */
  const char *bind;
  uintptr_t bind_len;
  /**
   * How long a dial may take before it gives up.
   */
  uint64_t connect_timeout_us;
  bool has_connect_timeout;
  /**
   * Happy Eyeballs: how long before the next address is also dialed.
   */
  uint64_t failover_delay_us;
  bool has_failover_delay;
  /**
   * Happy Eyeballs: how long the first family waits for the AAAA answer.
   */
  uint64_t resolution_delay_us;
  bool has_resolution_delay;
  /**
   * Whether the WebSocket fallback may be raced, for a UDP-blocked network.
   * Enabled unless you turn it off, hence the flag.
   */
  bool websocket_enabled;
  bool has_websocket_enabled;
  /**
   * How long QUIC gets before the WebSocket fallback is also dialed.
   */
  uint64_t websocket_delay_us;
  bool has_websocket_delay;
  /**
   * Accept any certificate. Development only: prefer `tls_fingerprints`,
   * and pairing this with a fingerprint or a root is rejected at dial.
   */
  bool tls_disable_verify;
  /**
   * Whether to trust the platform root store. Its default depends on the
   * backend, so it needs the flag to distinguish "off" from "unset".
   */
  bool tls_system_roots;
  bool has_tls_system_roots;
  /**
   * Extra root certificate paths to trust.
   */
  const struct moq_string *tls_roots;
  uintptr_t tls_roots_len;
  /**
   * SHA-256 certificate fingerprints to pin, hex encoded. The native
   * equivalent of the browser's `serverCertificateHashes`.
   */
  const struct moq_string *tls_fingerprints;
  uintptr_t tls_fingerprints_len;
  /**
   * SNI override, or NULL to use the host from the URL.
   */
  const char *tls_host_name;
  uintptr_t tls_host_name_len;
  /**
   * Client certificate and key paths for mTLS, or NULL for none.
   */
  const char *tls_cert;
  uintptr_t tls_cert_len;
  const char *tls_key;
  uintptr_t tls_key_len;
  /**
   * Reconnect pacing. Each must leave a non-zero delay or retrying would
   * spin, which is rejected at dial.
   */
  uint64_t backoff_initial_us;
  bool has_backoff_initial;
  uint32_t backoff_multiplier;
  bool has_backoff_multiplier;
  uint64_t backoff_max_us;
  bool has_backoff_max;
  /**
   * How long reconnection keeps trying before giving up for good.
   */
  uint64_t backoff_timeout_us;
  bool has_backoff_timeout;
  /**
   * QUIC transport tuning, all ignored by the WebSocket fallback.
   */
  uint64_t quic_max_streams;
  bool has_quic_max_streams;
  uint64_t quic_idle_timeout_us;
  bool has_quic_idle_timeout;
  uint64_t quic_keep_alive_us;
  bool has_quic_keep_alive;
  /**
   * Generic segmentation offload and path MTU discovery. Both default to the
   * backend's choice, so both need their flag.
   */
  bool quic_gso;
  bool has_quic_gso;
  bool quic_mtu_discovery;
  bool has_quic_mtu_discovery;
  /**
   * Congestion control family name, or NULL for the backend's choice.
   */
  const char *quic_congestion_control;
  uintptr_t quic_congestion_control_len;
  /**
   * Directory to write qlog traces into, or NULL for none. Capture is
   * compile-time optional; see [moq_qlog_supported].
   */
  const char *quic_qlog;
  uintptr_t quic_qlog_len;
} moq_client_config;

/**
 * A callback receiving a positive handle/value, zero on clean completion, or a negative error.
 */
typedef void (*moq_status_callback)(void *user_data, int32_t code);

/**
 * A snapshot of connection statistics, filled in by [moq_session_stats].
 *
 * Each metric has a `*_valid` flag: when `false`, the matching value is meaningless because
 * the transport backend doesn't report it (a `false` flag is NOT the same as a zero value).
 * Native QUIC reports every metric; the browser WebTransport reports few or none. Initialize
 * the struct to zero before the call; [moq_session_stats] overwrites every field.
 */
typedef struct moq_connection_stats {
  /**
   * Smoothed round-trip time, in microseconds.
   */
  uint64_t rtt_us;
  bool rtt_valid;
  /**
   * Estimated send bandwidth from the congestion controller, in bits per second.
   */
  uint64_t estimated_send_rate_bps;
  bool estimated_send_rate_valid;
  /**
   * Estimated receive bandwidth from MoQ PROBE, in bits per second.
   */
  uint64_t estimated_recv_rate_bps;
  bool estimated_recv_rate_valid;
  /**
   * Total bytes sent, including retransmissions and overhead.
   */
  uint64_t bytes_sent;
  bool bytes_sent_valid;
  /**
   * Total bytes received, including duplicates and overhead.
   */
  uint64_t bytes_received;
  bool bytes_received_valid;
  /**
   * Total bytes lost (detected via retransmission or acknowledgement).
   */
  uint64_t bytes_lost;
  bool bytes_lost_valid;
  /**
   * Total datagrams sent.
   */
  uint64_t packets_sent;
  bool packets_sent_valid;
  /**
   * Total datagrams received.
   */
  uint64_t packets_received;
  bool packets_received_valid;
  /**
   * Total datagrams detected as lost.
   */
  uint64_t packets_lost;
  bool packets_lost_valid;
} moq_connection_stats;

/**
 * Statistics and protocol sampled from the same connection by [moq_session_snapshot].
 */
typedef struct moq_connection_snapshot {
  /**
   * Transport statistics, with per-metric availability flags.
   */
  struct moq_connection_stats stats;
  /**
   * Negotiated draft name, backed by static storage valid for the process lifetime.
   */
  struct moq_string protocol;
} moq_connection_snapshot;

/**
 * Settings for [moq_server_listen].
 *
 * Zero it and set only what you need; new settings are appended, and a zeroed
 * one keeps the previous behavior. TLS is required: set `tls_cert` and `tls_key`,
 * or `tls_generate`.
 */
typedef struct moq_server_config {
  /**
   * Address to bind, e.g. `[::]:443`, `127.0.0.1:0`, or `localhost:4443`; NULL
   * for `[::]:443`. A port of 0 picks one; read it back with [moq_server_addr].
   */
  const char *bind;
  uintptr_t bind_len;
  /**
   * Certificate chain paths (PEM), paired with `tls_key`.
   */
  const struct moq_string *tls_cert;
  uintptr_t tls_cert_len;
  /**
   * Private key paths (PEM), paired with `tls_cert`.
   */
  const struct moq_string *tls_key;
  uintptr_t tls_key_len;
  /**
   * Hostnames to generate a self-signed certificate for. Clients must pin its
   * fingerprint ([moq_server_fingerprints]) or disable verification.
   */
  const struct moq_string *tls_generate;
  uintptr_t tls_generate_len;
} moq_server_config;

/**
 * A route advertisement: hops and costs.
 *
 * Pair with [moq_publish_announce] or [moq_origin_dynamic]. Zeroed (NULL hops,
 * hops_len 0, cost 0) is the default route. `hops` is borrowed for the duration
 * of the call that reads it.
 *
 * `cost` is the warm price: what pulling via this route costs today, lower
 * wins. `cold` is the same path undiscounted; when `has_cold` is false it
 * defaults to `cost`, which is what a publisher seeding its production cost
 * wants. New fields always append, so a zeroed struct keeps meaning the
 * defaults.
 */
typedef struct moq_route {
  /**
   * Hop ids, oldest first. NULL when `hops_len` is 0. 0 is the anonymous
   * mark and is legal on a received chain.
   */
  const uint64_t *hops;
  uintptr_t hops_len;
  /**
   * Preference among routes covering the same prefix: lower wins.
   */
  uint64_t cost;
  /**
   * The same path with every warm discount removed. Ignored unless `has_cold`.
   */
  uint64_t cold;
  /**
   * Whether `cold` applies. When false, `cold` defaults to `cost`.
   */
  bool has_cold;
} moq_route;

/**
 * A route announcement or retraction from an origin.
 */
typedef struct moq_announce_update {
  /**
   * The covered prefix, relative to the origin, NOT NULL terminated
   */
  const char *prefix;
  uintptr_t prefix_len;
  /**
   * What each requested filter wildcard matched. Each string is NOT NULL terminated.
   * Meaningful only when `has_captures` is true; false means the route overlaps
   * the filter without pinning every wildcard.
   */
  const struct moq_string *captures;
  uintptr_t captures_len;
  bool has_captures;
  /**
   * Whether the route is active or was retracted
   * This MUST toggle between true and false over the lifetime of the route
   */
  bool active;
} moq_announce_update;

/**
 * Configuration for [moq_publish_audio].
 *
 * Zero the struct, then set `format` and the required `init` bytes. New
 * optional fields are appended so existing initializers keep their meaning.
 */
typedef struct moq_audio_init {
  /**
   * The audio codec, a [moq_audio_format] value.
   */
  uint32_t format;
  /**
   * Codec init bytes: an OpusHead, an AudioSpecificConfig, a STREAMINFO.
   * Required, since audio has no in-band config to resolve from frames.
   */
  const uint8_t *init;
  /**
   * Length of `init` in bytes.
   */
  uintptr_t init_len;
  /**
   * Human-readable rendition name for track pickers, or NULL if not used.
   */
  const char *label;
  /**
   * Length of `label` in bytes.
   */
  uintptr_t label_len;
} moq_audio_init;

/**
 * Optional catalog fields for [moq_video_init::hint].
 *
 * Zero the struct and set only the `has_*` flags you want. Hints fill gaps the
 * bitstream leaves (especially bitrate); a value the stream detects later wins
 * for dimensions.
 */
typedef struct moq_video_hint {
  /**
   * Encoded width in pixels when `has_coded` is true.
   */
  uint32_t coded_width;
  /**
   * Encoded height in pixels when `has_coded` is true.
   */
  uint32_t coded_height;
  /**
   * Whether `coded_width` and `coded_height` are present.
   */
  bool has_coded;
  /**
   * Maximum bitrate in bits per second when `has_bitrate` is true.
   */
  uint64_t bitrate;
  /**
   * Whether `bitrate` is present.
   */
  bool has_bitrate;
  /**
   * Frame rate when `has_framerate` is true.
   */
  double framerate;
  /**
   * Whether `framerate` is present.
   */
  bool has_framerate;
  /**
   * Latency-optimized decode when `has_optimize_for_latency` is true.
   */
  bool optimize_for_latency;
  /**
   * Whether `optimize_for_latency` is present.
   */
  bool has_optimize_for_latency;
} moq_video_hint;

/**
 * Configuration for [moq_publish_video].
 *
 * Zero the struct, then set `format` and whatever else the codec needs. `init`
 * may stay NULL for a format that resolves in band.
 */
typedef struct moq_video_init {
  /**
   * The video codec, a [moq_video_format] value.
   */
  uint32_t format;
  /**
   * Codec init bytes (an avcC, an hvcC), or NULL for a format that resolves
   * from the stream itself.
   */
  const uint8_t *init;
  /**
   * Length of `init` in bytes.
   */
  uintptr_t init_len;
  /**
   * Human-readable rendition name for track pickers, or NULL if not used.
   */
  const char *label;
  /**
   * Length of `label` in bytes.
   */
  uintptr_t label_len;
  /**
   * Catalog fields the bitstream cannot reveal itself. Zeroed means none.
   */
  struct moq_video_hint hint;
} moq_video_init;

/**
 * Configuration for [moq_publish_container].
 *
 * There is no label here: a container publishes and describes its own tracks,
 * so a rendition name would have no single track to land on.
 */
typedef struct moq_container_init {
  /**
   * The container format, a [moq_container_format] value.
   */
  uint32_t format;
  /**
   * The leading chunk of the container, decoded immediately, or NULL.
   */
  const uint8_t *init;
  /**
   * Length of `init` in bytes.
   */
  uintptr_t init_len;
} moq_container_init;

/**
 * Catalog properties shared by every video rendition.
 *
 * A false `has_*` flag clears that field from the next catalog rather than preserving its previous value.
 */
typedef struct moq_video_properties {
  /**
   * Final rendered width in pixels when `has_display` is true.
   */
  uint32_t display_width;
  /**
   * Final rendered height in pixels when `has_display` is true.
   */
  uint32_t display_height;
  /**
   * Whether `display_width` and `display_height` are present.
   */
  bool has_display;
  /**
   * Clockwise rotation in degrees when `has_rotation` is true.
   */
  double rotation;
  /**
   * Whether `rotation` is present.
   */
  bool has_rotation;
  /**
   * Whether to flip horizontally after rotation when `has_flip` is true.
   */
  bool flip;
  /**
   * Whether `flip` is present.
   */
  bool has_flip;
} moq_video_properties;

/**
 * The container of a video or audio rendition, plus whatever that container
 * needs to describe itself.
 *
 * Zeroing this struct means `MOQ_CONTAINER_KIND_LEGACY` with no init segment,
 * which is what a rendition written by [moq_publish_audio] or [moq_publish_video] carries.
 */
typedef struct moq_container {
  /**
   * `moq_container_kind` discriminant.
   */
  uint32_t kind;
  /**
   * The CMAF init segment (ftyp+moov), or NULL.
   * Read only when `kind` is `MOQ_CONTAINER_KIND_CMAF`, where it is required.
   */
  const uint8_t *init;
  uintptr_t init_len;
} moq_container;

/**
 * Information about a video rendition in the catalog.
 */
typedef struct moq_video_config {
  /**
   * The name of the track, NOT NULL terminated.
   */
  const char *name;
  uintptr_t name_len;
  /**
   * The codec of the track, NOT NULL terminated
   */
  const char *codec;
  uintptr_t codec_len;
  /**
   * The description of the track, or NULL if not used.
   * This is codec specific, for example H264:
   *   - NULL: annex.b encoded
   *   - Non-NULL: AVCC encoded
   */
  const uint8_t *description;
  uintptr_t description_len;
  /**
   * The encoded width/height of the media, a hint so a decoder can size its
   * buffers up front. Zero means absent, which no valid dimension is, so the
   * two are independent: a catalog carrying only one round-trips unchanged.
   */
  uint32_t coded_width;
  uint32_t coded_height;
  /**
   * How the track's frames are wrapped.
   */
  struct moq_container container;
  /**
   * Human-readable rendition name for track pickers, or NULL if not used.
   */
  const char *label;
  /**
   * Length of `label` in bytes.
   */
  uintptr_t label_len;
} moq_video_config;

/**
 * Information about an audio rendition in the catalog.
 */
typedef struct moq_audio_config {
  /**
   * The name of the track, NOT NULL terminated
   */
  const char *name;
  uintptr_t name_len;
  /**
   * The codec of the track, NOT NULL terminated
   */
  const char *codec;
  uintptr_t codec_len;
  /**
   * The description of the track, or NULL if not used.
   */
  const uint8_t *description;
  uintptr_t description_len;
  /**
   * The sample rate of the track in Hz
   */
  uint32_t sample_rate;
  /**
   * The number of channels in the track
   */
  uint32_t channel_count;
  /**
   * How the track's frames are wrapped.
   */
  struct moq_container container;
  /**
   * Human-readable rendition name for track pickers, or NULL if not used.
   */
  const char *label;
  /**
   * Length of `label` in bytes.
   */
  uintptr_t label_len;
} moq_audio_config;

/**
 * Publisher-side raw track properties.
 *
 * A null [moq_publish_track] `info` pointer uses the moq-net defaults.
 * A zero-initialized struct also uses those defaults, except `priority`, which
 * has no presence flag: zero is the least urgent, and 127 is the moq-net default.
 */
typedef struct moq_track_info {
  /**
   * Priority, used to break ties between subscriptions of equal subscriber priority.
   */
  uint8_t priority;
  /**
   * Maximum age of a non-latest group before the publisher evicts it, in microseconds.
   * The publisher-side half of `moq_subscription.max_age_us`.
   */
  uint64_t max_age_us;
  /**
   * Whether `max_age_us` is set. When false, the publisher's default applies.
   */
  bool max_age_present;
  /**
   * Per-frame timescale in ticks per second.
   */
  uint64_t timescale;
  /**
   * Whether `timescale` is set. When false, the default microsecond timescale
   * applies, matching the `timestamp_us` units used everywhere else in this ABI.
   */
  bool timescale_present;
} moq_track_info;

/**
 * Options for a JSON snapshot track (lossy latest-value mode).
 *
 * The same config is passed to a producer and its consumers, but the consumer reads only
 * `compression`; `delta_ratio` is producer-only.
 */
typedef struct moq_json_snapshot_config {
  /**
   * How aggressively the producer emits deltas instead of full snapshots. `0` disables deltas
   * (one snapshot per group); a positive value allows roughly that many snapshots' worth of
   * deltas before rolling. Ignored by the consumer.
   */
  uint32_t delta_ratio;
  /**
   * DEFLATE-compress each group. Must match on the producer and consumer.
   */
  bool compression;
} moq_json_snapshot_config;

/**
 * Options for a JSON stream track (lossless append-log mode).
 */
typedef struct moq_json_stream_config {
  /**
   * DEFLATE-compress the group. Must match on the producer and consumer.
   */
  bool compression;
} moq_json_stream_config;

/**
 * Options for a binary data track, in either mode.
 *
 * The mode is fixed by which constructor is called ([moq_publish_binary_snapshot] or
 * [moq_publish_binary_stream]), so it is not in here.
 */
typedef struct moq_binary_config {
  /**
   * DEFLATE-compress each payload, advertised in the catalog entry.
   */
  bool compression;
  /**
   * The payloads' media type (e.g. `image/jpeg`), or NULL to leave it unstated.
   */
  const char *mime;
  /**
   * Length of `mime` in bytes.
   */
  uintptr_t mime_len;
} moq_binary_config;

/**
 * One untyped application catalog section: a name and its JSON value.
 *
 * Both `name` and `json` are UTF-8, NOT NULL terminated, and borrow the catalog
 * snapshot's storage. They stay valid until the snapshot is freed with
 * [moq_consume_catalog_free]. `json` is the section's value serialized as JSON
 * (parse it yourself); a top-level catalog key beyond `video`/`audio`.
 */
typedef struct moq_section {
  /**
   * The section name, NOT NULL terminated.
   */
  const char *name;
  uintptr_t name_len;
  /**
   * The section value as a JSON document, NOT NULL terminated.
   */
  const char *json;
  uintptr_t json_len;
} moq_section;

/**
 * Information about a frame of media.
 */
typedef struct moq_frame {
  /**
   * The payload of the frame, or NULL/0 if the stream has ended
   */
  const uint8_t *payload;
  uintptr_t payload_size;
  /**
   * The presentation timestamp of the frame in microseconds
   */
  uint64_t timestamp_us;
  /**
   * Whether this frame opens a group or is a video keyframe; audio is true only at a group start.
   */
  bool keyframe;
} moq_frame;

/**
 * Subscriber-side raw track delivery preferences.
 *
 * A null [moq_consume_track] or [moq_consume_track_update] `subscription`
 * pointer uses the moq-net defaults.
 */
typedef struct moq_subscription {
  /**
   * Delivery priority. Higher values preempt lower ones under contention.
   */
  uint8_t priority;
  /**
   * Maximum age of a non-latest group before it is skipped, in microseconds.
   * Zero skips immediately. Enforced by the publisher's cache and by any local buffering.
   */
  uint64_t max_age_us;
  /**
   * The lowest group to deliver (a floor). A floor is not a request: `max_age_us` is
   * what asks for data, and delivery starts at the oldest group at or above the floor
   * within that budget (the latest group at the default budget of 0).
   */
  uint64_t group_start;
  /**
   * Whether `group_start` is present. When false, there is no floor.
   */
  bool group_start_present;
  /**
   * First group not to deliver (exclusive), or ignored when `group_end_present` is
   * false. `0` is the empty range.
   */
  uint64_t group_end;
  /**
   * Whether `group_end` is present. When false, there is no end cap.
   */
  bool group_end_present;
} moq_subscription;

/**
 * A best-effort raw track datagram delivered via [moq_consume_datagrams].
 */
typedef struct moq_datagram {
  /**
   * The payload of the datagram, or NULL/0 if the track has ended.
   */
  const uint8_t *payload;
  uintptr_t payload_size;
  /**
   * The presentation timestamp of the datagram in microseconds.
   */
  uint64_t timestamp_us;
  /**
   * Per-track sequence number, drawn from the same namespace as groups.
   */
  uint64_t sequence;
} moq_datagram;

/**
 * A JSON value delivered by a consumer callback.
 */
typedef struct moq_json_value {
  /**
   * The JSON document as UTF-8, NOT NULL terminated.
   */
  const char *json;
  uintptr_t json_len;
} moq_json_value;

/**
 * PCM layout the caller hands to [`moq_encode_audio_frame`].
 */
typedef struct moq_audio_encoder_input {
  /**
   * `moq_audio_sample_format` discriminant.
   */
  uint32_t format;
  uint32_t sample_rate;
  /**
   * Interleaved channel count, which also names the speaker layout by the
   * WAVE convention: 1 mono, 2 stereo, 3 2.1, 4 quad, 5 5.0, 6 5.1, 7 6.1,
   * 8 7.1, in front left, front right, center, LFE, back, side order.
   */
  uint32_t channels;
} moq_audio_encoder_input;

/**
 * Codec-side configuration. `sample_rate` / `channels` = 0 means
 * "match the input (snapping the rate up to a libopus-supported
 * value if necessary)".
 */
typedef struct moq_audio_encoder_output {
  /**
   * Codec id, UTF-8: "opus", "pcm", or "aac". AAC encodes through the
   * platform's encoder, so a host without one refuses it.
   */
  const char *codec;
  uintptr_t codec_len;
  /**
   * 0 = derive from input.
   */
  uint32_t sample_rate;
  /**
   * 0 = derive from input.
   */
  uint32_t channels;
  /**
   * 0 = libopus default.
   */
  uint32_t bitrate;
  /**
   * Encoded frame duration in microseconds. Opus accepts exactly
   * 2500/5000/10000/20000/40000/60000 us. 0 = the codec's default: 20 ms for
   * Opus, which matches the JS publish path, and 1024 samples for AAC.
   */
  uint32_t frame_duration_us;
} moq_audio_encoder_output;

/**
 * One audio frame: payload bytes plus a presentation timestamp.
 *
 * `data` is owned by the consume slab (see
 * [`moq_decode_audio_frame_free`]) or borrowed by the publish call
 * (the publisher copies before returning).
 */
typedef struct moq_audio_frame {
  uint64_t timestamp_us;
  const uint8_t *data;
  uintptr_t data_size;
} moq_audio_frame;

/**
 * PCM layout the caller wants out of [`moq_decode_audio`].
 */
typedef struct moq_audio_decoder_output {
  uint32_t format;
  /**
   * 0 = deliver at the codec's native sample rate.
   */
  uint32_t sample_rate;
  /**
   * 0 = deliver at the codec's native channel count. A count names its
   * layout as `moq_audio_encoder_input.channels` describes, and the decoder
   * remixes to it.
   */
  uint32_t channels;
  /**
   * Upper bound on buffering before skipping a stalled group, in
   * microseconds. Same congestion-control knob as
   * `moq_consume_audio`'s `max_age_us`. 0 = skip
   * aggressively (the moq-mux default); set to your playout
   * buffer (tens to a few hundred ms) for a softer skip. Named
   * `_max` to leave room for a future `min_buffer_us`, a
   * jitter-buffer floor rather than a staleness bound.
   */
  uint64_t max_age_us;
} moq_audio_decoder_output;

/**
 * Raw frame layout the caller hands to [`moq_encode_video_frame`], plus
 * the resolution and rate the encoder is opened at. Every published frame must
 * match `width` x `height`; scale before publishing if your source moves.
 */
typedef struct moq_video_encoder_input {
  /**
   * `moq_video_pixel_format` discriminant.
   */
  uint32_t format;
  /**
   * Encoded width in pixels. Must be even (I420 chroma is subsampled 2x2).
   */
  uint32_t width;
  /**
   * Encoded height in pixels. Must be even.
   */
  uint32_t height;
  /**
   * Nominal frames per second, used for the codec time base and the default
   * bitrate and keyframe interval. Must be non-zero.
   */
  uint32_t framerate;
} moq_video_encoder_input;

/**
 * Codec-side configuration for [`moq_encode_video`]. Every knob spells
 * "unset" as 0.
 */
typedef struct moq_video_encoder_output {
  /**
   * `moq_video_codec` discriminant.
   */
  uint32_t codec;
  /**
   * Target bitrate in bits per second. 0 derives one from the resolution and
   * framerate.
   */
  uint64_t bitrate;
  /**
   * Keyframe interval in frames: a subscriber joining mid-stream waits at
   * most this many frames before it can decode. 0 uses ~2 seconds.
   */
  uint32_t gop;
  /**
   * `moq_video_encoder_kind` discriminant.
   */
  uint32_t kind;
  /**
   * Backend name, UTF-8, e.g. `"videotoolbox"`, `"nvenc"`, `"mediafoundation"`,
   * `"openh264"`. Read only when `kind` is `MOQ_VIDEO_ENCODER_KIND_NAMED`.
   */
  const char *encoder;
  uintptr_t encoder_len;
} moq_video_encoder_output;

/**
 * One raw frame handed to [`moq_encode_video_frame`].
 *
 * Pixel format and resolution are fixed by [`moq_video_encoder_input`] at
 * publish time, so a frame carries neither: `data` is exactly one picture in
 * that layout, borrowed for the duration of the call (the encoder copies before
 * returning). The decode side has its own [`moq_video_frame`], which does carry
 * dimensions, since there they are what the stream turned out to be.
 */
typedef struct moq_video_encoder_frame {
  /**
   * Presentation timestamp, in microseconds.
   */
  uint64_t timestamp_us;
  const uint8_t *data;
  uintptr_t data_size;
} moq_video_encoder_frame;

/**
 * Decode-side configuration the caller passes to [`moq_decode_video`].
 *
 * `format` selects the CPU pixel layout of each [`moq_video_frame`] (`I420`
 * is `width * height * 3 / 2` bytes, `RGBA` is `width * height * 4` bytes),
 * and `width`/`height` select its size: zero both for the stream's native
 * size, otherwise both must be even and non-zero.
 *
 * This struct is versioned by recompilation, not by reserved fields: adding a
 * field changes its layout, so rebuild callers against the `moq.h` that ships
 * with the `libmoq.a` they link.
 */
typedef struct moq_video_decoder_output {
  /**
   * Upper bound on buffering before skipping a stalled group, in
   * microseconds. Same congestion-control knob as
   * `moq_consume_video`'s `max_age_us`. 0 = skip aggressively
   * (the moq-mux default); set to your playout buffer for a softer skip.
   */
  uint64_t max_age_us;
  /**
   * `moq_video_pixel_format` discriminant. Unknown values fail
   * [`moq_decode_video`] rather than decoding into an assumed layout.
   */
  uint32_t format;
  /**
   * Target width in pixels. 0 with `height` 0 means the native size.
   */
  uint32_t width;
  /**
   * Target height in pixels. 0 with `width` 0 means the native size.
   */
  uint32_t height;
} moq_video_decoder_output;

/**
 * One decoded video frame from [`moq_decode_video`]: pixels plus a
 * presentation timestamp.
 *
 * The pixel layout is what [`moq_video_decoder_output`]'s `format` asked
 * for: I420 is the Y plane (`width * height`), then U, then V (`width/2 *
 * height/2` each), no row padding, BT.601 limited range; RGBA is tightly
 * packed `width * height * 4` bytes, no row padding.
 *
 * `data` is owned by the consume slab and stays valid until the same id is
 * released with [`moq_decode_video_frame_free`].
 *
 * The publish side has its own [`moq_video_encoder_frame`], which carries no
 * dimensions because the encoder already fixed them.
 */
typedef struct moq_video_frame {
  uint64_t timestamp_us;
  uint32_t width;
  uint32_t height;
  const uint8_t *data;
  uintptr_t data_size;
} moq_video_frame;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Initialize the library with a log level.
 *
 * This should be called before any other functions.
 * The log_level is a string: "error", "warn", "info", "debug", "trace"
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that level is a valid pointer to level_len bytes of data.
 */
int32_t moq_log_level(const char *level, uintptr_t level_len);

/**
 * Human-readable reason for the most recent failed call on the calling thread.
 *
 * libmoq functions return only a negative code; this exposes the matching message
 * (including detail the code can't carry, e.g. which URL failed to parse or why a
 * decode failed). The string is only meaningful after a call returned a negative
 * code; check the code first.
 *
 * Returns a NUL-terminated, UTF-8 pointer valid until the next libmoq call **on the
 * same thread**, or NULL if no error has been recorded on this thread. Copy it if you
 * need it to outlive the next call. Errors delivered through status callbacks carry
 * their code directly; read this from inside the callback to get their reason.
 */
const char *moq_error(void);

/**
 * Structured protocol details for the most recent failed call on the calling thread.
 *
 * When that failure was a session close or stream reset, writes the scope, verbatim
 * wire code, and recognized kind into `out` and returns 0. Returns a negative code
 * (and leaves `out` untouched) when the last error was not a protocol failure
 * (transport, not-found, a bad handle, ...). Do not parse [moq_error] for this.
 *
 * The values are only meaningful after a call returned a negative code; check the
 * code first. Same lifetime as [moq_error]: overwritten by the next libmoq call on
 * this thread. Errors delivered through status callbacks are recorded before the
 * callback runs, so read this from inside the callback.
 *
 * # Safety
 * - The caller must ensure that `out` is a valid pointer to a [moq_protocol_error].
 */
int32_t moq_error_protocol(struct moq_protocol_error *out);

/**
 * List the protocol versions offered during the handshake by default.
 *
 * Writes up to `count` names into `dst` and returns the total number available, which
 * may be larger than `count`. Pass a NULL `dst` with a zero `count` to size the array
 * first. Each name borrows a static string valid for the life of the process, so a
 * caller building a menu can hold them indefinitely.
 *
 * Work-in-progress versions are omitted, since they are not advertised unless pinned;
 * a dial still accepts them by name.
 *
 * Returns the total count on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is either NULL with a zero `count`, or a valid
 *   pointer to `count` writable [moq_string] values.
 */
int32_t moq_versions(struct moq_string *dst, uintptr_t count);

/**
 * Whether this build can capture qlog traces.
 *
 * Capture is compile-time optional. [moq_client_config]'s `quic_qlog` accepts a directory
 * either way, but dialing fails when the support is absent, so a caller offering the
 * knob should hide it rather than surface an option that cannot work.
 */
bool moq_qlog_supported(void);

/**
 * The settings [moq_session_connect] dials with when given NULL.
 *
 * Behaviorally the same as a zeroed struct, so this is for display rather than
 * for dialing: a settings UI can show the real numbers instead of hardcoding
 * ones that go stale when a default is retuned. The knobs whose default depends
 * on the backend (GSO, path MTU discovery, congestion control, the TLS root
 * store) come back with their `has_*` flag false, since there is no single value
 * to report.
 *
 * Returned by value because there is nothing to fail: no handle to look up and
 * no pointer to reject. Prefer a zeroed struct when you only mean to set a knob
 * or two, and this when you want to read the numbers.
 */
struct moq_client_config moq_client_defaults(void);

/**
 * Start establishing a connection to a MoQ server.
 *
 * Takes origin handles, which are used for publishing and consuming broadcasts respectively.
 * - Any broadcasts in `origin_publish` will be announced to the server.
 * - Any broadcasts announced by the server will be available in `origin_consume`.
 * - If an origin handle is 0, that functionality is completely disabled.
 *
 * This may be called multiple times to connect to different servers.
 * Origins can be shared across sessions, useful for fanout or relaying.
 *
 * Pass NULL for `config` to dial with the defaults. Fill in a
 * [moq_client_config] to pin a protocol version, adjust TLS trust, or tune the
 * transport; it is read during the call and not retained, so the same one can
 * dial any number of sessions.
 *
 * Returns a non-zero handle to the session on success, or a negative code on (immediate) failure.
 * You should call [moq_session_close], even on error, to free up resources.
 *
 * The session reconnects automatically with exponential backoff if the connection drops.
 * Published broadcasts are re-announced and consumers re-subscribed on each reconnect,
 * since the origins outlive the underlying connection.
 *
 * `on_status` reports the session lifecycle through its status code:
 * - `> 0` on every (re)connect, carrying the connection epoch (`1` = first connect,
 *   `2` = first reconnect, and so on), so a reconnect is distinguishable from the
 *   initial connect. May fire repeatedly. Transient disconnects are not reported.
 * - `0` when the session is closed cleanly via [moq_session_close] (terminal).
 * - a negative error code if reconnection permanently gives up, e.g. the backoff
 *   timeout is exceeded (terminal).
 *
 * After a terminal (`<= 0`) status, `on_status` is never called again and `user_data`
 * is never touched again, so that final callback is the point to release `user_data`.
 * The terminal `0` fires even after [moq_session_close], so do not free `user_data` on
 * the close call itself.
 *
 * # Safety
 * - The caller must ensure that url is a valid pointer to url_len bytes of data.
 * - `config` must be NULL, or an aligned, readable [moq_client_config]. Every
 *   non-NULL pointer inside it must be valid for its paired length, and all of
 *   them must stay alive for the duration of this call: the config is read
 *   here, not copied by whoever filled it in.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_status` callback.
 */
int32_t moq_session_connect(const char *url,
                            uintptr_t url_len,
                            const struct moq_client_config *config,
                            uint32_t origin_publish,
                            uint32_t origin_consume,
                            moq_status_callback on_status,
                            void *user_data);

/**
 * Request that a session shut down.
 *
 * Returns immediately: zero on success, or a negative code if the session is
 * unknown or already closing. Does NOT free `user_data`. The
 * [moq_session_connect] `on_status` callback still fires once more with a
 * terminal `0` (or a negative error), and that final callback is where
 * `user_data` should be released. Safe to call from any thread, including from
 * within `on_status`.
 */
int32_t moq_session_close(uint32_t session);

/**
 * Snapshot the current connection statistics for a session.
 *
 * Fills `dst` with a point-in-time view of the underlying QUIC/WebTransport connection
 * (RTT, bandwidth estimates, byte/packet counters). Each metric carries a `*_valid` flag
 * since availability depends on the transport backend; see [moq_connection_stats].
 *
 * Returns zero on success, or a negative code on failure: the session handle is unknown, or
 * the session is currently reconnecting and has no live connection (in which case `dst` is
 * left untouched). Safe to call repeatedly to poll stats over the life of the session.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_connection_stats] struct.
 */
int32_t moq_session_stats(uint32_t session, struct moq_connection_stats *dst);

/**
 * Snapshot statistics and the negotiated protocol from the same live connection.
 *
 * Returns zero on success, or a negative code when the handle is unknown or offline
 * between reconnects. On failure, `dst` is untouched. The protocol string points at
 * static storage valid for the process lifetime and must not be freed.
 *
 * # Safety
 * - `dst` must point at a writable [moq_connection_snapshot] struct.
 */
int32_t moq_session_snapshot(uint32_t session, struct moq_connection_snapshot *dst);

/**
 * Listen for incoming sessions.
 *
 * Binds before returning, so a bad address or certificate fails here with a reason in
 * [moq_error]. Returns a non-zero server handle on success, or a negative code on failure.
 *
 * `on_request` is called with a positive session request handle for each incoming
 * session, then exactly once more with a terminal code: `0` (stopped cleanly, including
 * after [moq_server_close]) or a negative error. After the terminal (`<= 0`) callback,
 * `user_data` is never touched again, so release it there. Answer each request with
 * [moq_session_request_accept], [moq_session_request_reject], or [moq_session_request_free].
 *
 * # Safety
 * - `config` must point at a readable [moq_server_config] whose non-NULL pointers are
 *   valid for their paired lengths during this call.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_request` callback.
 */
int32_t moq_server_listen(const struct moq_server_config *config,
                          moq_status_callback on_request,
                          void *user_data);

/**
 * The address a server bound, e.g. `127.0.0.1:4443`.
 *
 * The destination borrows the server's storage, valid until its terminal
 * `on_request` callback. Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable [moq_string].
 */
int32_t moq_server_addr(uint32_t server, struct moq_string *dst);

/**
 * The SHA-256 fingerprints of the server's certificates, hex encoded.
 *
 * Pin these on a client (`tls_fingerprints` in [moq_client_config], or a browser's
 * `serverCertificateHashes`) to trust a generated certificate. Writes up to `count`
 * into `dst` and returns the total number available; pass a NULL `dst` with a zero
 * `count` to size the array first. Each string borrows the server's storage, valid
 * until its terminal `on_request` callback.
 *
 * Returns the total count on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must be NULL with a zero `count`, or point to `count` writable [moq_string] values.
 */
int32_t moq_server_fingerprints(uint32_t server, struct moq_string *dst, uintptr_t count);

/**
 * Stop listening.
 *
 * Returns immediately: zero on success, or a negative code if the server is unknown or
 * already closing. Sessions already accepted keep running. The `on_request` callback
 * still fires once more with a terminal `0` after the sockets are released, so the
 * address can be bound again from there; release `user_data` in that callback.
 */
int32_t moq_server_close(uint32_t server);

/**
 * The path of a session request, without the query, or empty for the root.
 *
 * The destination borrows the request's storage: copy it out before accept,
 * reject, or [moq_session_request_free]. Returns a zero on success, or a negative
 * code on failure.
 *
 * # Safety
 * - `dst` must point at a writable [moq_string].
 */
int32_t moq_session_request_path(uint32_t request, struct moq_string *dst);

/**
 * The query of a session request without the leading `?`, or a NULL `data` if it has none.
 *
 * Where a token usually rides. The destination borrows the request's storage, like
 * [moq_session_request_path]. Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable [moq_string].
 */
int32_t moq_session_request_query(uint32_t request, struct moq_string *dst);

/**
 * Accept a session request, completing the MoQ handshake.
 *
 * Takes origin handles like [moq_session_connect]: broadcasts in `origin_publish` are
 * announced to the peer, and broadcasts the peer announces land in `origin_consume`.
 * An origin handle of 0 disables that direction.
 *
 * Consumes the request handle on success and returns a non-zero session handle, or a
 * negative code on failure (leaving the request unanswered). The session works with every
 * `moq_session_*` call; stats and bandwidth report offline until the handshake completes.
 *
 * `on_status` reports the session lifecycle:
 * - `1` once the handshake completes. An accepted session is a single connection, so it
 *   never reconnects and never reports more.
 * - `0` when closed via [moq_session_close] (terminal).
 * - a negative error code if the handshake fails or the peer closes the session; read
 *   [moq_error_protocol] for the peer's close code (terminal).
 *
 * # Safety
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_status` callback.
 */
int32_t moq_session_request_accept(uint32_t request,
                                   uint32_t origin_publish,
                                   uint32_t origin_consume,
                                   moq_status_callback on_status,
                                   void *user_data);

/**
 * Reject a session request with an HTTP-style status code.
 *
 * 401 and 403 are sent as the protocol's unauthorized close; every other code is sent
 * as an application error. Consumes the request handle. Returns a zero on success, or
 * a negative code on failure.
 */
int32_t moq_session_request_reject(uint32_t request, uint16_t code);

/**
 * Free a session request without accepting or rejecting it.
 *
 * Dropping the request closes the session. Returns a zero on success, or a negative
 * code if the handle is unknown.
 */
int32_t moq_session_request_free(uint32_t request);

/**
 * Create an origin for publishing broadcasts.
 *
 * Origins contain any number of broadcasts addressed by path.
 * The same broadcast can be published to multiple origins under different paths.
 *
 * [moq_origin_announced] can be used to discover broadcasts published to this origin.
 * This is extremely useful for discovering what is available on the server to [moq_origin_request].
 *
 * Returns a non-zero handle to the origin on success.
 */
int32_t moq_origin_create(void);

/**
 * Create a broadcast at `path` on an origin, for publishing media tracks.
 *
 * The broadcast is invisible and unroutable, on this origin and its peers
 * alike, until [moq_publish_announce]. Fill it with the `moq_publish_*`
 * functions, then announce it. [moq_publish_close] ends it for good.
 *
 * Returns a non-zero broadcast handle on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that path is a valid pointer to path_len bytes of data.
 */
int32_t moq_origin_create_broadcast(uint32_t origin, const char *path, uintptr_t path_len);

/**
 * Advertise `prefix` and serve the requests beneath it.
 *
 * A route claims `prefix` and every path beneath it (the empty prefix claims
 * every path). A service that only serves some of them advertises the
 * covering prefix and rejects the rest as they are requested. `on_request` is
 * required: a NULL callback is refused before the route is advertised. It is
 * invoked with a positive request handle for each
 * pending broadcast, then exactly once more with a terminal code: `0` (stopped
 * cleanly, including after [moq_origin_dynamic_cancel]) or a negative error.
 * After the terminal (`<= 0`) callback, `user_data` is never touched again.
 *
 * Returns a non-zero handle on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that prefix is a valid pointer to prefix_len bytes of data.
 * - `route` may be NULL, or must point at a readable [moq_route].
 * - `on_request` must be non-NULL; a missing callback is refused before the route is advertised.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_request` callback.
 */
int32_t moq_origin_dynamic(uint32_t origin,
                           const char *prefix,
                           uintptr_t prefix_len,
                           const struct moq_route *route,
                           moq_status_callback on_request,
                           void *user_data);

/**
 * Re-price a served route in place. The prefix cannot change.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `route` may be NULL, or must point at a readable [moq_route].
 */
int32_t moq_origin_dynamic_update(uint32_t dynamic, const struct moq_route *route);

/**
 * Stop serving and retract the route.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * The [moq_origin_dynamic] `on_request` callback still fires once more with a
 * terminal `0` (or a negative error), and that final callback is where
 * `user_data` should be released.
 */
int32_t moq_origin_dynamic_cancel(uint32_t dynamic);

/**
 * The path of a broadcast request delivered to a [moq_origin_dynamic] callback.
 *
 * The destination borrows the request's storage: copy it out before accept,
 * reject, or [moq_broadcast_request_free].
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable [moq_string].
 */
int32_t moq_broadcast_request_path(uint32_t request, struct moq_string *dst);

/**
 * Accept a broadcast request with an unannounced broadcast producer.
 *
 * Consumes the request handle. Returns a zero on success, or a negative code
 * on failure.
 */
int32_t moq_broadcast_request_accept(uint32_t request, uint32_t broadcast);

/**
 * Reject a broadcast request with an application error code.
 *
 * Consumes the request handle. Returns a zero on success, or a negative code
 * on failure.
 */
int32_t moq_broadcast_request_reject(uint32_t request, uint16_t error_code);

/**
 * Free a broadcast request without accepting or rejecting it.
 *
 * Dropping the request rejects it. Returns a zero on success, or a negative
 * code if the handle is unknown.
 */
int32_t moq_broadcast_request_free(uint32_t request);

/**
 * Learn about broadcasts matching a pattern scope under an origin.
 *
 * `prefix` is a literal path root. `filter` is a pattern relative to that
 * prefix, or NULL for every path beneath it. Empty is a valid exact filter.
 * Delivered [moq_announce_update] prefixes remain relative to the origin.
 *
 * `on_announce` is invoked with a positive announced ID for each broadcast,
 * then exactly once more with a terminal code: `0` (stopped cleanly) or a
 * negative error. After the terminal (`<= 0`) callback, `on_announce` is never
 * called again and `user_data` is never touched again, so release `user_data`
 * there. The terminal callback fires even after [moq_origin_announced_cancel].
 *
 * - [moq_origin_announced_info] is used to query information about the broadcast.
 * - [moq_origin_announced_free] releases each delivered announced ID once read.
 * - [moq_origin_announced_cancel] is used to stop receiving announcements.
 *
 * Returns a non-zero handle on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_announce` callback.
 */
int32_t moq_origin_announced(uint32_t origin,
                             const char *prefix,
                             uintptr_t prefix_len,
                             const char *filter,
                             uintptr_t filter_len,
                             moq_status_callback on_announce,
                             void *user_data);

/**
 * Query information about a broadcast discovered by [moq_origin_announced].
 *
 * The destination is filled with the route information. The `prefix`, `captures`,
 * and capture string pointers borrow the announcement's storage: copy them out
 * before calling [moq_origin_announced_free], which invalidates them.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_announce_update] struct.
 */
int32_t moq_origin_announced_info(uint32_t announced, struct moq_announce_update *dst);

/**
 * Free a single announcement delivered to a [moq_origin_announced] `on_announce` callback.
 *
 * Each announce / unannounce event hands the callback a distinct announcement handle (read
 * with [moq_origin_announced_info]); release it here once done to avoid leaking one per event
 * over the life of the listener. This is per-announcement and distinct from
 * [moq_origin_announced_cancel], which stops the listener itself. After freeing,
 * any pointer obtained from [moq_origin_announced_info] for this handle is dangling.
 *
 * Returns zero on success, or a negative code if the handle is unknown.
 */
int32_t moq_origin_announced_free(uint32_t announced);

/**
 * Stop receiving announcements for broadcasts published to an origin.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`. The [moq_origin_announced] `on_announce` callback
 * still fires once more with a terminal `0` (or a negative error), and that
 * final callback is where `user_data` should be released.
 */
int32_t moq_origin_announced_cancel(uint32_t announced);

/**
 * Consume a broadcast from an origin by path, waiting until something can serve it.
 *
 * Resolves against future announcements: it waits for the announcement to arrive (e.g. over the
 * network) and then delivers the broadcast handle via `on_broadcast`. Use it right after
 * [moq_session_connect] to avoid racing announcement gossip. To resolve against only what is
 * reachable now, use [moq_origin_request] instead. A broadcast created on this origin
 * resolves once it is announced, like a remote one.
 *
 * `on_broadcast` is invoked with a positive broadcast handle once announced, then exactly once
 * more with a terminal code: `0` (the wait finished, including after
 * [moq_origin_announced_broadcast_cancel]) or a negative error. After the terminal (`<= 0`) callback,
 * `on_broadcast` is never called again and `user_data` is never touched again, so release
 * `user_data` there. The broadcast handle is usable with [moq_consume_catalog] / [moq_consume_track]
 * and must be freed separately with [moq_consume_close].
 *
 * Returns a non-zero handle to the wait on success, or a negative code on (immediate) failure.
 *
 * # Safety
 * - The caller must ensure that path is a valid pointer to path_len bytes of data.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_broadcast` callback.
 */
int32_t moq_origin_announced_broadcast(uint32_t origin,
                                       const char *path,
                                       uintptr_t path_len,
                                       moq_status_callback on_broadcast,
                                       void *user_data);

/**
 * Abort a wait started by [moq_origin_announced_broadcast].
 *
 * Returns immediately: zero on success, or a negative code if already closed. Does NOT free
 * `user_data`. The [moq_origin_announced_broadcast] `on_broadcast` callback still fires once more
 * with a terminal `0` (or a negative error), and that final callback is where `user_data` should
 * be released. Any broadcast handle already delivered is unaffected and must still be freed with
 * [moq_consume_close].
 */
int32_t moq_origin_announced_broadcast_cancel(uint32_t task);

/**
 * Request a broadcast from an origin by path, resolving as soon as it can be served.
 *
 * Resolves against what is announced *now*, where [moq_origin_announced_broadcast] waits
 * indefinitely: it returns an announced broadcast at once, and fails when none is reachable,
 * including a broadcast created but not announced. It does NOT wait for a later
 * announcement. Serve on-demand paths with [moq_origin_dynamic].
 *
 * `on_broadcast` is invoked with a positive broadcast handle once served, then exactly once more
 * with a terminal code: `0` (finished, including after [moq_origin_request_cancel]) or a negative
 * error. After the terminal (`<= 0`) callback, `user_data` is never touched again, so release it
 * there. The broadcast handle is usable with [moq_consume_catalog] / [moq_consume_track] and must
 * be freed separately with [moq_consume_close].
 *
 * Returns a non-zero handle to the request on success, or a negative code on (immediate) failure.
 *
 * # Safety
 * - The caller must ensure that path is a valid pointer to path_len bytes of data.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_broadcast` callback.
 */
int32_t moq_origin_request(uint32_t origin,
                           const char *path,
                           uintptr_t path_len,
                           moq_status_callback on_broadcast,
                           void *user_data);

/**
 * Abort a request started by [moq_origin_request].
 *
 * Returns immediately: zero on success, or a negative code if already closed. Does NOT free
 * `user_data`; the [moq_origin_request] `on_broadcast` callback fires once more with a terminal
 * code, which is where `user_data` should be released. Any broadcast handle already delivered is
 * unaffected and must still be freed with [moq_consume_close].
 */
int32_t moq_origin_request_cancel(uint32_t task);

/**
 * Close an origin and clean up its resources.
 *
 * The origin keeps running while a broadcast published on it or a
 * [moq_origin_dynamic] handler lives; close or cancel those to end it.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_origin_close(uint32_t origin);

/**
 * Advertise a broadcast's exact path as a route.
 *
 * Announcing again re-prices the route in place. A NULL `route` uses the default
 * (no hops, cost 0). Until announced, the broadcast is invisible and unroutable for
 * local consumers and peers alike.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `route` may be NULL, or must point at a readable [moq_route].
 */
int32_t moq_publish_announce(uint32_t broadcast, const struct moq_route *route);

/**
 * Retract a broadcast's exact-path advertisement, if any.
 *
 * Local consumers and peers alike stop discovering and requesting it; tracks already in
 * flight carry on, and announcing again brings it back. Returns a zero on success, or a
 * negative code on failure.
 */
int32_t moq_publish_unannounce(uint32_t broadcast);

/**
 * End a broadcast for good and release its handle.
 *
 * The origin retracts the path immediately and serves no new tracks; tracks already
 * subscribed carry on to their own end. The handle is invalid afterwards, so closing
 * it again fails like any unknown handle.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_close(uint32_t broadcast);

/**
 * Deprecated: use [moq_publish_close]. A broadcast end carries no cause.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_finish(uint32_t broadcast);

/**
 * Publish one audio codec as a new media track.
 *
 * The track is named after the format (`0.opus`), so a subscriber finds it
 * through the catalog rather than by a name you choose.
 * [moq_audio_init::init] is required: audio resolves its whole rendition from
 * those bytes. Frames written with [moq_publish_media_frame] must be in decode
 * order.
 *
 * Returns a non-zero handle to the track on success, or a negative code on failure.
 *
 * # Safety
 * - `config` must be NULL, or point to an aligned, readable [moq_audio_init].
 *   Every non-NULL pointer inside it must be valid for its paired length and
 *   stay alive for the duration of this call. A NULL config is rejected with an
 *   ordinary error.
 */
int32_t moq_publish_audio(uint32_t broadcast, const struct moq_audio_init *config);

/**
 * Publish one video codec as a new media track.
 *
 * Named as in [moq_publish_audio]. [moq_video_init::init] may be NULL for a
 * format that resolves in band.
 *
 * Returns a non-zero handle to the track on success, or a negative code on failure.
 *
 * # Safety
 * - As [moq_publish_audio], for a [moq_video_init].
 */
int32_t moq_publish_video(uint32_t broadcast, const struct moq_video_init *config);

/**
 * Publish a container, which demuxes and publishes its own tracks.
 *
 * Feed it whole chunks with [moq_publish_container_write]. Unlike the codec
 * entry points there is no label: a container describes each track it publishes
 * from its own metadata.
 *
 * Returns a non-zero handle to the container on success, or a negative code on failure.
 *
 * # Safety
 * - As [moq_publish_audio], for a [moq_container_init].
 */
int32_t moq_publish_container(uint32_t broadcast, const struct moq_container_init *config);

/**
 * Draw a group boundary on a media importer.
 *
 * For a codec track this ends the open group; the next frame written starts a new one. Audio has
 * no boundary of its own (every packet is independently decodable), so this is the only thing
 * that gives it groups: call it after every frame for one group (one QUIC stream) the relay
 * forwards without waiting, or at a segment cadence to align with video for HLS/DASH. Video
 * groups at its own keyframes and needs this only to override that.
 *
 * A container has its own [moq_publish_container_cut], since it rolls a group on every track it
 * publishes rather than ending one group.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_media_cut(uint32_t media);

/**
 * Draw a group boundary and number the next group `sequence`.
 *
 * [moq_publish_media_cut] with an explicit sequence, for a caller whose group numbers have to be
 * deterministic: two encoders publishing the same content align per GOP so a consumer can fail
 * over between them.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_media_seek(uint32_t media, uint64_t sequence);

/**
 * Finish a media track, flushing any buffered frames. No more frames can be written.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_media_finish(uint32_t export_);

/**
 * Watch whether a media track has subscribers, so an encoder runs only while someone watches.
 *
 * `on_demand` fires right away with the current [moq_demand] state, again on every
 * change, then exactly once more with a terminal code: `0` (the track ended or the
 * watcher was stopped with [moq_publish_demand_cancel]) or a negative error. After the
 * terminal (`<= 0`) callback, `user_data` is never touched again. Reporting the current
 * state first means a track that went unused before the watcher existed still reports it.
 *
 * A container handle is refused: it publishes several tracks and has no single demand.
 *
 * Returns a non-zero watcher handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_demand` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_demand` callback.
 */
int32_t moq_publish_media_demand(uint32_t media, moq_status_callback on_demand, void *user_data);

/**
 * Stop a demand watcher from [moq_publish_track_demand], [moq_publish_media_demand],
 * [`crate::moq_encode_video_demand`], or [`crate::moq_encode_audio_demand`].
 *
 * Returns immediately: zero on success, or a negative code if already closed. The
 * watcher's `on_demand` callback still fires once more with a terminal `0`, and
 * that final callback is where `user_data` should be released.
 */
int32_t moq_publish_demand_cancel(uint32_t watcher);

/**
 * Write a whole chunk of container bytes.
 *
 * No timestamp: a container carries its tracks' timing itself, and the importer
 * reads it out rather than taking the caller's word for it.
 *
 * Returns zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `payload` is valid for `payload_size` bytes.
 */
int32_t moq_publish_container_write(uint32_t container,
                                    const uint8_t *payload,
                                    uintptr_t payload_size);

/**
 * Declare that the next chunk starts a new segment, rolling a group on every
 * track the container publishes.
 *
 * An fMP4 source carrying `styp` atoms declares its own segments, so this is
 * only needed when it doesn't. Formats with no segment concept (MKV, TS, FLV)
 * ignore it.
 *
 * Returns zero on success, or a negative code on failure.
 */
int32_t moq_publish_container_cut(uint32_t container);

/**
 * Start a new segment and number its groups `sequence`.
 *
 * Returns zero on success, or a negative code on failure.
 */
int32_t moq_publish_container_seek(uint32_t container, uint64_t sequence);

/**
 * Finish every track the container publishes and release the handle.
 *
 * Returns zero on success, or a negative code on failure.
 */
int32_t moq_publish_container_finish(uint32_t container);

/**
 * Write data to a track.
 *
 * The encoding of `data` depends on the track `format`.
 * The timestamp is in microseconds.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that payload is a valid pointer to payload_size bytes of data.
 */
int32_t moq_publish_media_frame(uint32_t media,
                                const uint8_t *payload,
                                uintptr_t payload_size,
                                uint64_t timestamp_us);

/**
 * Record the transport handoff of one locally encoded frame for catalog jitter.
 *
 * `timestamp_us` is the frame's presentation time on the broadcast media clock. Call this
 * after [moq_publish_media_frame] only for local encoder output; file, pipe, and network imports
 * must remain clock-free. The monotonic handoff time is sampled inside this process.
 *
 * Returns zero on success, or a negative code for an invalid handle or timestamp.
 */
int32_t moq_publish_media_flush(uint32_t media, uint64_t timestamp_us);

/**
 * Mark a timeline break and restart handoff measurement without lowering advertised jitter.
 *
 * Publishes a discontinuity marker; resumed frames must continue the broadcast media clock,
 * and video must resume on a keyframe.
 * Returns zero on success, or a negative code on failure.
 */
int32_t moq_publish_media_discontinuity(uint32_t media);

/**
 * Replace the catalog properties shared by every video rendition.
 *
 * Rotation is clockwise and normalized to the nearest quarter turn. A field whose matching `has_*` flag is false is removed from the next catalog update.
 *
 * Returns zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `properties` points to a valid [moq_video_properties].
 */
int32_t moq_publish_video_properties(uint32_t broadcast,
                                     const struct moq_video_properties *properties);

/**
 * Add or replace a video rendition in a broadcast's catalog.
 *
 * This is the producer counterpart to [moq_consume_video_config]: instead of
 * reading a rendition out of a catalog, it writes one into the catalog of a
 * broadcast created with [moq_origin_create_broadcast]. The rendition is keyed by
 * `config.name`; calling this again with the same name replaces the rendition
 * you declared, so a config can be refined in place. It fails only when a
 * [moq_publish_video] track owns the name, since that track publishes and
 * retires its own rendition. The updated catalog is published to subscribers
 * automatically.
 *
 * The struct fields are read as inputs:
 * - `name` / `codec` are required (NOT NULL terminated) string slices.
 * - `label` may be NULL to omit the human-readable rendition name.
 * - `description` may be NULL to omit it.
 * - `coded_width` / `coded_height` may be zero to omit them.
 * - `container` describes how the frames written to the track are wrapped. A
 *   zeroed one declares the legacy container, which is what [moq_publish_video]
 *   writes; declare CMAF or LOC for a [moq_publish_track] whose frames you
 *   already encode that way.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `config` points to a valid [moq_video_config].
 * - The caller must ensure each non-NULL pointer inside `config` is valid for its length.
 */
int32_t moq_publish_video_config(uint32_t broadcast, const struct moq_video_config *config);

/**
 * Add or replace an audio rendition in a broadcast's catalog.
 *
 * This is the producer counterpart to [moq_consume_audio_config]. The rendition
 * is keyed by `config.name`, on the same terms as [moq_publish_video_config]:
 * a repeat call replaces your own rendition, and a name a [moq_publish_audio]
 * track owns is refused. The updated catalog is published to subscribers
 * automatically.
 *
 * The struct fields are read as inputs:
 * - `name` / `codec` are required (NOT NULL terminated) string slices.
 * - `label` may be NULL to omit the human-readable rendition name.
 * - `sample_rate` / `channel_count` are required.
 * - `description` may be NULL to omit it.
 * - `container` describes how the frames written to the track are wrapped, the
 *   same as for [moq_publish_video_config].
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `config` points to a valid [moq_audio_config].
 * - The caller must ensure each non-NULL pointer inside `config` is valid for its length.
 */
int32_t moq_publish_audio_config(uint32_t broadcast, const struct moq_audio_config *config);

/**
 * Remove a video rendition from a broadcast's catalog by name.
 *
 * Removes a rendition added by [moq_publish_video_config]. Any other name is a
 * no-op, including one a [moq_publish_video] track owns, which is retired by
 * [moq_publish_media_finish] instead. The updated catalog is published to
 * subscribers automatically.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 */
int32_t moq_publish_video_remove(uint32_t broadcast, const char *name, uintptr_t name_len);

/**
 * Remove an audio rendition from a broadcast's catalog by name.
 *
 * Same rules as [moq_publish_video_remove].
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 */
int32_t moq_publish_audio_remove(uint32_t broadcast, const char *name, uintptr_t name_len);

/**
 * Set (or replace) a top-level application catalog section by name.
 *
 * This is the producer counterpart to [moq_consume_catalog_section] /
 * [moq_consume_catalog_section_at]: it writes an arbitrary top-level JSON key into the
 * catalog of a broadcast created with [moq_origin_create_broadcast], beyond the
 * `video`/`audio` keys owned by the media pipeline. Calling it again with the
 * same name replaces the section. The updated catalog is published to
 * subscribers automatically.
 *
 * `json` is a JSON document (object, array, string, ...) as `json_len` bytes of
 * UTF-8. Returns a zero on success, or a negative code on failure: invalid JSON
 * yields a Json error (-37); a reserved `name` (`video`/`audio`) yields a mux error.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 * - The caller must ensure that json is a valid pointer to json_len bytes of data.
 */
int32_t moq_publish_catalog_section(uint32_t broadcast,
                                    const char *name,
                                    uintptr_t name_len,
                                    const char *json,
                                    uintptr_t json_len);

/**
 * Remove a top-level application catalog section by name.
 *
 * This is a no-op if no section with that name exists. The updated catalog is
 * published to subscribers automatically.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 */
int32_t moq_publish_catalog_section_remove(uint32_t broadcast,
                                           const char *name,
                                           uintptr_t name_len);

/**
 * Create a raw track on a broadcast for arbitrary byte payloads.
 *
 * Unlike [moq_publish_audio] and [moq_publish_video], this is the bare moq-net primitive: no
 * codec, container, or catalog framing. Frames written to it are delivered
 * as-is to subscribers using [moq_consume_track]. Use it for non-media tracks
 * (control channels, JSON metadata, etc.), or pair it with
 * [moq_publish_video_config] / [moq_publish_audio_config] to also describe the
 * track in the catalog. Pass NULL for `info` to use moq-net defaults.
 *
 * Returns a non-zero handle to the track on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 * - The caller must ensure that info is either NULL or a valid pointer to a [moq_track_info] struct.
 */
int32_t moq_publish_track(uint32_t broadcast,
                          const char *name,
                          uintptr_t name_len,
                          const struct moq_track_info *info);

/**
 * Append a new group to a raw track, returning a group producer.
 *
 * Groups are delivered independently and each may contain any number of frames
 * written via [moq_publish_group_frame]. Sequence numbers auto-increment.
 *
 * Returns a non-zero handle to the group on success, or a negative code on failure.
 */
int32_t moq_publish_track_group(uint32_t track);

/**
 * Create a raw group with an explicit sequence number.
 *
 * Returns a non-zero group handle on success, or a negative code on failure.
 */
int32_t moq_publish_track_group_at(uint32_t track, uint64_t sequence);

/**
 * Write a single-frame group to a raw track with a timestamp.
 *
 * Convenience for the common one-frame-per-group pattern. Equivalent to
 * appending a group, writing one frame, and finishing it.
 * The timestamp is in microseconds.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that payload is a valid pointer to payload_size bytes of data.
 */
int32_t moq_publish_track_frame(uint32_t track,
                                const uint8_t *payload,
                                uintptr_t payload_size,
                                uint64_t timestamp_us);

/**
 * Send a best-effort datagram on a raw track created by [moq_publish_track].
 *
 * Takes `payload` then `timestamp_us`, matching [moq_publish_track_frame]. The payload must
 * be at most 1200 bytes. On success the datagram's per-track sequence number (shared with the
 * group namespace) is written to `out_sequence` when it is non-NULL. Datagrams are
 * delivered only on transports and wire versions with a datagram channel; there is no
 * group fallback.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that payload is a valid pointer to payload_size bytes of data.
 * - `out_sequence` must be NULL or a valid pointer to a `uint64_t`.
 */
int32_t moq_publish_track_datagram(uint32_t track,
                                   const uint8_t *payload,
                                   uintptr_t payload_size,
                                   uint64_t timestamp_us,
                                   uint64_t *out_sequence);

/**
 * Finish a raw track. No more groups or frames can be written.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_track_finish(uint32_t track);

/**
 * Declare a raw track's exclusive final group sequence.
 *
 * Groups below `final_sequence` may still be created. Groups at or above it
 * are rejected. The track remains open for groups below the boundary. Call
 * [moq_publish_track_finish] after producing the remaining groups.
 */
int32_t moq_publish_track_finish_at(uint32_t track, uint64_t final_sequence);

/**
 * Abort a raw track with an application error code.
 */
int32_t moq_publish_track_abort(uint32_t track, uint16_t error_code);

/**
 * Watch whether a raw track has subscribers. See [moq_publish_media_demand] for the
 * callback contract.
 *
 * Returns a non-zero watcher handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_demand` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_demand` callback.
 */
int32_t moq_publish_track_demand(uint32_t track, moq_status_callback on_demand, void *user_data);

/**
 * Serve subscriber requests for tracks the broadcast has not declared.
 *
 * Without a live handler a subscription to an unknown track name is refused. While one
 * is live, `on_request` is invoked with a positive request handle for each pending
 * track, then exactly once more with a terminal code: `0` (the broadcast finished, or
 * [moq_publish_dynamic_cancel] was called) or a negative error. After the terminal
 * (`<= 0`) callback, `user_data` is never touched again. Answer each request with
 * [moq_track_request_accept], [moq_track_request_video], [moq_track_request_audio],
 * or [moq_track_request_abort]; the subscriber waits until you do.
 *
 * Returns a non-zero handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_request` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_request` callback.
 */
int32_t moq_publish_dynamic(uint32_t broadcast, moq_status_callback on_request, void *user_data);

/**
 * Serve fetches of groups a raw track no longer has cached.
 *
 * Without a live handler a fetch that misses the cache fails as not found. While one is
 * live, `on_group` is invoked with a positive group-request handle for each miss, then
 * exactly once more with a terminal code: `0` (the track ended, or
 * [moq_publish_dynamic_cancel] was called) or a negative error. After the terminal
 * (`<= 0`) callback, `user_data` is never touched again. Cached groups never reach the
 * handler. Answer each request with [moq_group_request_accept] or [moq_group_request_abort].
 *
 * Returns a non-zero handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_group` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_group` callback.
 */
int32_t moq_publish_track_dynamic(uint32_t track, moq_status_callback on_group, void *user_data);

/**
 * Stop a request handler from [moq_publish_dynamic], [moq_publish_track_dynamic], or
 * [moq_track_request_dynamic]. Requests not yet delivered are rejected.
 *
 * Returns immediately: zero on success, or a negative code if already closed. The
 * handler's callback still fires once more with a terminal `0`, and that final
 * callback is where `user_data` should be released.
 */
int32_t moq_publish_dynamic_cancel(uint32_t dynamic);

/**
 * The name of a track request delivered to a [moq_publish_dynamic] callback.
 *
 * The destination borrows the request's storage: copy it out before accepting,
 * aborting, or freeing the request.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable [moq_string].
 */
int32_t moq_track_request_name(uint32_t request, struct moq_string *dst);

/**
 * Serve fetches of uncached groups on a requested track, before accepting it.
 *
 * A track requested by a fetch has that group pending from birth. Register the
 * handler here, before [moq_track_request_accept], so the request survives the
 * transition; the callback contract is that of [moq_publish_track_dynamic].
 *
 * Returns a non-zero handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_group` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_group` callback.
 */
int32_t moq_track_request_dynamic(uint32_t request, moq_status_callback on_group, void *user_data);

/**
 * Accept a track request as a raw track, resolving the waiting subscribers.
 *
 * Consumes the request handle. `info` is as in [moq_publish_track]: NULL for the
 * microsecond default. Returns a non-zero track handle usable with every
 * `moq_publish_track_*` function, or a negative code on failure.
 *
 * # Safety
 * - `info` must be NULL or a valid pointer to a [moq_track_info] struct.
 */
int32_t moq_track_request_accept(uint32_t request, const struct moq_track_info *info);

/**
 * Accept a track request as an audio track, the importer picking the timescale.
 *
 * Consumes the request handle. Returns the same kind of media handle as
 * [moq_publish_audio], or a negative code on failure.
 *
 * # Safety
 * - As [moq_publish_audio], for `config`.
 */
int32_t moq_track_request_audio(uint32_t request, const struct moq_audio_init *config);

/**
 * Accept a track request as a video track, the importer picking the timescale.
 *
 * Consumes the request handle. Returns the same kind of media handle as
 * [moq_publish_video], or a negative code on failure.
 *
 * # Safety
 * - As [moq_publish_audio], for a [moq_video_init].
 */
int32_t moq_track_request_video(uint32_t request, const struct moq_video_init *config);

/**
 * Reject a track request with an application error code, failing the waiting subscribers.
 *
 * Consumes the request handle. Returns a zero on success, or a negative code on failure.
 */
int32_t moq_track_request_abort(uint32_t request, uint16_t error_code);

/**
 * Free a track request without accepting it, which rejects it.
 *
 * Returns a zero on success, or a negative code if the handle is unknown.
 */
int32_t moq_track_request_free(uint32_t request);

/**
 * The group sequence a group request asks for.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable `uint64_t`.
 */
int32_t moq_group_request_sequence(uint32_t request, uint64_t *dst);

/**
 * The delivery priority the fetching consumer asked for.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable `uint8_t`.
 */
int32_t moq_group_request_priority(uint32_t request, uint8_t *dst);

/**
 * The first frame of the group the fetch wants; 0 is the whole group.
 *
 * [moq_group_request_accept] positions the returned producer here, so frames you
 * write keep the indices they have in the group rather than restarting at 0. Read
 * this to know which frames to fetch from storage.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - `dst` must point at a writable `uint64_t`.
 */
int32_t moq_group_request_frame_start(uint32_t request, uint64_t *dst);

/**
 * Accept a group request, resolving the waiting fetches with the group you then fill.
 *
 * Consumes the request handle. The returned producer starts at
 * [moq_group_request_frame_start], so the first frame you write lands at that
 * index. Returns a non-zero group handle usable with [moq_publish_group_frame]
 * and [moq_publish_group_finish], or a negative code on failure, including when
 * the group is already cached.
 */
int32_t moq_group_request_accept(uint32_t request);

/**
 * Reject a group request with an application error code, failing the waiting fetches.
 *
 * Consumes the request handle. Returns a zero on success, or a negative code on failure.
 */
int32_t moq_group_request_abort(uint32_t request, uint16_t error_code);

/**
 * Free a group request without accepting it, which rejects it.
 *
 * Returns a zero on success, or a negative code if the handle is unknown.
 */
int32_t moq_group_request_free(uint32_t request);

/**
 * Write a frame into a raw group created by [moq_publish_track_group].
 *
 * The timestamp is in microseconds.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that payload is a valid pointer to payload_size bytes of data.
 */
int32_t moq_publish_group_frame(uint32_t group,
                                const uint8_t *payload,
                                uintptr_t payload_size,
                                uint64_t timestamp_us);

/**
 * Finish a raw group. No more frames can be written.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_group_finish(uint32_t group);

/**
 * Abort a raw group with an application error code.
 */
int32_t moq_publish_group_abort(uint32_t group, uint16_t error_code);

/**
 * Create a JSON snapshot track (lossy latest-value) on a broadcast.
 *
 * Values published via [moq_publish_json_snapshot_update] reach subscribers as a single latest
 * state; a late joiner only sees the newest. The track is advertised in the broadcast's catalog
 * under `json.tracks.<name>` with `mode: snapshot` (and `compression: deflate` when set), and the
 * entry is retired when the track finishes or fails, so consumers discover it with no extra call.
 *
 * Returns a non-zero handle to the JSON producer on success, or a negative code on failure,
 * including a mux error when the catalog already carries an entry named `name`.
 *
 * # Safety
 * - The caller must ensure `name` is a valid pointer to `name_len` bytes and `config` a valid pointer.
 */
int32_t moq_publish_json_snapshot(uint32_t broadcast,
                                  const char *name,
                                  uintptr_t name_len,
                                  const struct moq_json_snapshot_config *config);

/**
 * Publish a new value to a JSON snapshot track. `value` is a UTF-8 JSON document. A no-op if
 * unchanged from the previous update.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `value` is a valid pointer to `value_len` bytes.
 */
int32_t moq_publish_json_snapshot_update(uint32_t json, const char *value, uintptr_t value_len);

/**
 * Finish a JSON snapshot track. No more values can be published.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_json_snapshot_finish(uint32_t json);

/**
 * Create a JSON stream track (lossless append-log) on a broadcast.
 *
 * Every record appended via [moq_publish_json_stream_append] is preserved and delivered in order.
 * The track is advertised in the broadcast's catalog under `json.tracks.<name>` with
 * `mode: stream`, for as long as the track lives.
 *
 * Returns a non-zero handle to the JSON stream producer on success, or a negative code on failure,
 * including a mux error when the catalog already carries an entry named `name`.
 *
 * # Safety
 * - The caller must ensure `name` is a valid pointer to `name_len` bytes and `config` a valid pointer.
 */
int32_t moq_publish_json_stream(uint32_t broadcast,
                                const char *name,
                                uintptr_t name_len,
                                const struct moq_json_stream_config *config);

/**
 * Append one record to a JSON stream track. `value` is a UTF-8 JSON document.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `value` is a valid pointer to `value_len` bytes.
 */
int32_t moq_publish_json_stream_append(uint32_t stream, const char *value, uintptr_t value_len);

/**
 * Finish a JSON stream track. No more records can be appended.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_json_stream_finish(uint32_t stream);

/**
 * Create a binary snapshot track (lossy latest-value) on a broadcast: each payload supersedes the
 * last, and a late joiner only sees the newest, e.g. the latest thumbnail of a camera.
 *
 * The track is advertised in the broadcast's catalog under `binary.tracks.<name>` with
 * `mode: snapshot` (plus `mime` and `compression` when set), and the entry is retired when the
 * track finishes or fails.
 *
 * Returns a non-zero handle to the binary producer on success, or a negative code on failure,
 * including a mux error when the catalog already carries an entry named `name`.
 *
 * # Safety
 * - The caller must ensure `name` is a valid pointer to `name_len` bytes and `config` a valid pointer.
 */
int32_t moq_publish_binary_snapshot(uint32_t broadcast,
                                    const char *name,
                                    uintptr_t name_len,
                                    const struct moq_binary_config *config);

/**
 * Publish a new payload to a binary snapshot track, superseding the last.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `payload` is a valid pointer to `payload_len` bytes.
 */
int32_t moq_publish_binary_snapshot_update(uint32_t binary,
                                           const uint8_t *payload,
                                           uintptr_t payload_len);

/**
 * Finish a binary snapshot track and retire its catalog entry. No more payloads can be published.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_binary_snapshot_finish(uint32_t binary);

/**
 * Create a binary stream track (lossless append-log) on a broadcast: every payload is preserved
 * and delivered in order.
 *
 * The track is advertised in the broadcast's catalog under `binary.tracks.<name>` with
 * `mode: stream` (plus `mime` and `compression` when set), for as long as the track lives.
 *
 * Returns a non-zero handle to the binary stream producer on success, or a negative code on
 * failure, including a mux error when the catalog already carries an entry named `name`.
 *
 * # Safety
 * - The caller must ensure `name` is a valid pointer to `name_len` bytes and `config` a valid pointer.
 */
int32_t moq_publish_binary_stream(uint32_t broadcast,
                                  const char *name,
                                  uintptr_t name_len,
                                  const struct moq_binary_config *config);

/**
 * Append one payload to a binary stream track.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `payload` is a valid pointer to `payload_len` bytes.
 */
int32_t moq_publish_binary_stream_append(uint32_t stream,
                                         const uint8_t *payload,
                                         uintptr_t payload_len);

/**
 * Finish a binary stream track and retire its catalog entry. No more payloads can be appended.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_publish_binary_stream_finish(uint32_t stream);

/**
 * Create a catalog consumer for a broadcast.
 *
 * `on_catalog` is invoked with a positive catalog ID for each catalog update
 * (usable to query video/audio track information), then exactly once more with
 * a terminal code: `0` (closed cleanly) or a negative error. After the terminal
 * (`<= 0`) callback, `on_catalog` is never called again and `user_data` is never
 * touched again, so release `user_data` there. The terminal callback fires even
 * after [moq_consume_catalog_cancel].
 *
 * Returns a non-zero handle on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_catalog` callback.
 */
int32_t moq_consume_catalog(uint32_t broadcast, moq_status_callback on_catalog, void *user_data);

/**
 * Stop a catalog consumer's background subscription.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`; the [moq_consume_catalog] callback still fires once
 * more with a terminal `0` (or a negative error), which is where `user_data`
 * should be released. Catalog snapshots previously delivered via the callback
 * remain valid until freed with [moq_consume_catalog_free].
 */
int32_t moq_consume_catalog_cancel(uint32_t catalog);

/**
 * Free a catalog snapshot received via the [moq_consume_catalog] callback.
 *
 * This releases the snapshot and invalidates any borrowed references (e.g. pointers
 * returned by [moq_consume_video_config] or [moq_consume_audio_config]).
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_consume_catalog_free(uint32_t catalog);

/**
 * Query information about a video track in a catalog.
 *
 * The destination is filled with the video track information. `dst->container`
 * says how the track's frames are wrapped; skip a rendition whose kind is
 * `MOQ_CONTAINER_KIND_UNKNOWN`, since this build cannot parse it.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_video_config] struct.
 * - The caller must ensure that `dst` is not used after [moq_consume_catalog_free] is called.
 */
int32_t moq_consume_video_config(uint32_t catalog, uint32_t index, struct moq_video_config *dst);

/**
 * Query whether the publisher recommends temporarily avoiding a video rendition.
 *
 * The track remains available. A false value also covers catalogs that omit the
 * optional field.
 *
 * Returns zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` points to properly aligned, writable storage for a `bool`.
 */
int32_t moq_consume_video_stalled(uint32_t catalog, uint32_t index, bool *dst);

/**
 * Query the catalog properties shared by every video rendition.
 *
 * The destination is filled by value and remains valid after the catalog snapshot is freed.
 * Inspect each `has_*` flag before reading its value.
 *
 * Returns zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` points to a valid [moq_video_properties].
 */
int32_t moq_consume_video_properties(uint32_t catalog, struct moq_video_properties *dst);

/**
 * Query information about an audio track in a catalog.
 *
 * The destination is filled with the audio track information. `dst->container`
 * says how the track's frames are wrapped; skip a rendition whose kind is
 * `MOQ_CONTAINER_KIND_UNKNOWN`, since this build cannot parse it.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_audio_config] struct.
 * - The caller must ensure that `dst` is not used after [moq_consume_catalog_free] is called.
 */
int32_t moq_consume_audio_config(uint32_t catalog, uint32_t index, struct moq_audio_config *dst);

/**
 * Number of untyped application catalog sections in a catalog snapshot.
 *
 * These are the top-level catalog keys beyond `video`/`audio`, carried through
 * verbatim. Iterate them by index with [moq_consume_catalog_section_at], or look one up
 * directly by name with [moq_consume_catalog_section].
 *
 * Returns the count (>= 0) on success, or a negative code on failure.
 */
int32_t moq_consume_catalog_section_count(uint32_t catalog);

/**
 * Query an application catalog section by index, keyed by name.
 *
 * Fills `dst` with the section's name and JSON value at `index`, in the range
 * `[0, moq_consume_catalog_section_count)`. Both pointers borrow the snapshot's storage
 * and stay valid until it is freed with [moq_consume_catalog_free].
 *
 * Returns a zero on success, or a negative code on failure (e.g. `index` out of
 * range).
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_section] struct.
 * - The caller must ensure that `dst` is not used after [moq_consume_catalog_free] is called.
 */
int32_t moq_consume_catalog_section_at(uint32_t catalog, uint32_t index, struct moq_section *dst);

/**
 * Look up an application catalog section by name.
 *
 * Fills `dst` with the section's JSON value (the document to parse yourself).
 * The pointer borrows the snapshot's storage and stays valid until it is freed
 * with [moq_consume_catalog_free].
 *
 * Returns a zero on success, or a negative code on failure: no section with that
 * name yields a not-found error.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 * - The caller must ensure that `dst` is a valid pointer to a [moq_string] struct.
 * - The caller must ensure that `dst` is not used after [moq_consume_catalog_free] is called.
 */
int32_t moq_consume_catalog_section(uint32_t catalog,
                                    const char *name,
                                    uintptr_t name_len,
                                    struct moq_string *dst);

/**
 * Consume a video track from a broadcast, delivering frames in order.
 *
 * - `max_age_us` controls the maximum amount of buffering allowed before skipping a GoP.
 * - `on_frame` is called with a positive frame ID per frame, then exactly once
 *   more with a terminal code: `0` (closed cleanly) or a negative error. After
 *   the terminal (`<= 0`) callback, `on_frame` is never called again and
 *   `user_data` is never touched again, so release `user_data` there. The
 *   terminal callback fires even after [moq_consume_video_cancel].
 *
 * Returns a non-zero handle to the track on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_frame` callback.
 */
int32_t moq_consume_video(uint32_t catalog,
                          uint32_t index,
                          uint64_t max_age_us,
                          moq_status_callback on_frame,
                          void *user_data);

/**
 * Stop a video track consumer's background task.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`; the [moq_consume_video] `on_frame` callback
 * still fires once more with a terminal `0` (or a negative error), which is
 * where `user_data` should be released.
 */
int32_t moq_consume_video_cancel(uint32_t track);

/**
 * Consume an audio track from a broadcast, emitting the frames in order.
 *
 * `on_frame` is called with a positive frame ID per frame, then exactly once
 * more with a terminal code: `0` (closed cleanly) or a negative error. After
 * the terminal (`<= 0`) callback, `on_frame` is never called again and
 * `user_data` is never touched again, so release `user_data` there. The
 * terminal callback fires even after [moq_consume_audio_cancel].
 * The `max_age_us` parameter controls how long to wait before skipping frames.
 *
 * Returns a non-zero handle to the track on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_frame` callback.
 */
int32_t moq_consume_audio(uint32_t catalog,
                          uint32_t index,
                          uint64_t max_age_us,
                          moq_status_callback on_frame,
                          void *user_data);

/**
 * Stop an audio track consumer's background task.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`; the [moq_consume_audio] `on_frame` callback
 * still fires once more with a terminal `0` (or a negative error), which is
 * where `user_data` should be released.
 */
int32_t moq_consume_audio_cancel(uint32_t track);

/**
 * Get a chunk of a frame's payload.
 *
 * Read the payload of a frame as a single contiguous slice.
 *
 * Frames are not chunked; the entire payload is delivered through `dst.payload` /
 * `dst.payload_size` in one call. The pointer is valid until [`moq_consume_frame_free`]
 * is called for this frame.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_frame] struct.
 */
int32_t moq_consume_frame(uint32_t frame, struct moq_frame *dst);

/**
 * Free a decoded frame delivered via a [moq_consume_video] or [moq_consume_audio] callback.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_consume_frame_free(uint32_t frame);

/**
 * Close a broadcast consumer and clean up its resources.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_consume_close(uint32_t consume);

/**
 * Subscribe to a raw track by name, delivering each frame's payload as-is.
 *
 * This is the counterpart to [moq_publish_track]: no catalog lookup or
 * container parsing. `on_frame` is called with a positive raw frame ID for each
 * frame in sequence order, then exactly once more with a terminal code: `0`
 * (closed cleanly) or a negative error. After the terminal (`<= 0`) callback,
 * `on_frame` is never called again and `user_data` is never touched again, so
 * release `user_data` there. The terminal callback fires even after
 * [moq_consume_track_cancel]. Read each frame with [moq_consume_track_frame] and
 * release it with [moq_consume_track_frame_free]. Pass NULL for `subscription`
 * to use moq-net defaults.
 *
 * Returns a non-zero handle to the track on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 * - The caller must ensure that subscription is either NULL or a valid pointer to a [moq_subscription] struct.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_frame` callback.
 */
int32_t moq_consume_track(uint32_t broadcast,
                          const char *name,
                          uintptr_t name_len,
                          const struct moq_subscription *subscription,
                          moq_status_callback on_frame,
                          void *user_data);

/**
 * Update a raw track subscription's delivery preferences.
 *
 * Pass NULL for `subscription` to reset to moq-net defaults.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that subscription is either NULL or a valid pointer to a [moq_subscription] struct.
 */
int32_t moq_consume_track_update(uint32_t track,
                                 const struct moq_subscription *subscription);

/**
 * Read a raw frame's payload delivered via the [moq_consume_track] callback.
 *
 * Fills `dst.payload` / `dst.payload_size`; the pointer is valid until the
 * frame is released with [moq_consume_frame_free]. `dst.timestamp_us` is the
 * frame presentation timestamp in microseconds. `dst.keyframe` is reported as
 * false because raw tracks do not parse codec metadata.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_frame] struct.
 */
int32_t moq_consume_track_frame(uint32_t frame, struct moq_frame *dst);

/**
 * Free a raw frame delivered via the [moq_consume_track] callback, releasing its payload.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_consume_track_frame_free(uint32_t frame);

/**
 * Stop a raw track consumer's background task.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`; the [moq_consume_track] `on_frame` callback still
 * fires once more with a terminal `0` (or a negative error), which is where
 * `user_data` should be released. Frames already delivered via the callback
 * remain valid until released with [moq_consume_track_frame_free].
 */
int32_t moq_consume_track_cancel(uint32_t track);

/**
 * Subscribe to a raw track's best-effort datagrams by name.
 *
 * The datagram counterpart to [moq_consume_track], on its own subscription. `on_datagram`
 * is called with a positive datagram ID for each datagram in arrival order, then exactly
 * once more with a terminal code: `0` (closed cleanly) or a negative error. After the
 * terminal (`<= 0`) callback, `on_datagram` is never called again and `user_data` is never
 * touched again, so release `user_data` there. The terminal callback fires even after
 * [moq_consume_datagrams_cancel]. Read each datagram with [moq_consume_datagram] and release
 * it with [moq_consume_datagram_free]. Datagrams arrive only over datagram-capable
 * transports on moq-transport or lite-05 and newer moq-lite; there is no stream fallback.
 *
 * Returns a non-zero handle to the subscription on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that name is a valid pointer to name_len bytes of data.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_datagram` callback.
 */
int32_t moq_consume_datagrams(uint32_t broadcast,
                              const char *name,
                              uintptr_t name_len,
                              moq_status_callback on_datagram,
                              void *user_data);

/**
 * Read a datagram delivered via the [moq_consume_datagrams] callback.
 *
 * Fills `dst.payload` / `dst.payload_size` (valid until the datagram is released with
 * [moq_consume_datagram_free]), plus `dst.timestamp_us` and `dst.sequence`.
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure that `dst` is a valid pointer to a [moq_datagram] struct.
 */
int32_t moq_consume_datagram(uint32_t datagram, struct moq_datagram *dst);

/**
 * Free a datagram delivered via the [moq_consume_datagrams] callback, releasing its payload.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_consume_datagram_free(uint32_t datagram);

/**
 * Stop a datagram subscription's background task.
 *
 * Returns immediately: zero on success, or a negative code if already closed. Does NOT free
 * `user_data`; the [moq_consume_datagrams] `on_datagram` callback still fires once more with a
 * terminal `0` (or a negative error), which is where `user_data` should be released. Datagrams
 * already delivered via the callback remain valid until released with [moq_consume_datagram_free].
 */
int32_t moq_consume_datagrams_cancel(uint32_t task);

/**
 * Subscribe to a JSON snapshot track (lossy latest-value) by name.
 *
 * `on_value` is called with a positive value ID for each new latest value; a consumer that
 * falls behind collapses the backlog and only sees the newest. It is called exactly once more
 * with a terminal `0` (track ended / closed) or a negative error, after which `user_data` is
 * never touched again, so release it there. Read each value with [moq_consume_json_value] and
 * release it with [moq_consume_json_value_free]. Pass the same compression the producer used.
 *
 * Returns a non-zero handle to the task on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `name` is a valid pointer to `name_len` bytes and `config` a valid pointer.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_value` callback.
 */
int32_t moq_consume_json_snapshot(uint32_t broadcast,
                                  const char *name,
                                  uintptr_t name_len,
                                  const struct moq_json_snapshot_config *config,
                                  moq_status_callback on_value,
                                  void *user_data);

/**
 * Subscribe to a JSON stream track (lossless append-log) by name.
 *
 * `on_value` is called with a positive value ID for each record, in order, then once more with
 * a terminal `0` or negative error where `user_data` should be released. Read each value with
 * [moq_consume_json_value] and release it with [moq_consume_json_value_free].
 *
 * Returns a non-zero handle to the task on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `name` is a valid pointer to `name_len` bytes and `config` a valid pointer.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_value` callback.
 */
int32_t moq_consume_json_stream(uint32_t broadcast,
                                const char *name,
                                uintptr_t name_len,
                                const struct moq_json_stream_config *config,
                                moq_status_callback on_value,
                                void *user_data);

/**
 * Read a JSON value delivered via a [moq_consume_json_snapshot] or [moq_consume_json_stream] callback.
 *
 * Fills `dst.json` / `dst.json_len`; the pointer is valid until the value is released with
 * [moq_consume_json_value_free].
 *
 * Returns a zero on success, or a negative code on failure.
 *
 * # Safety
 * - The caller must ensure `dst` is a valid pointer to a [moq_json_value] struct.
 */
int32_t moq_consume_json_value(uint32_t value,
                               struct moq_json_value *dst);

/**
 * Release a JSON value delivered via a consumer callback.
 *
 * Returns a zero on success, or a negative code on failure.
 */
int32_t moq_consume_json_value_free(uint32_t value);

/**
 * Stop a JSON consumer's background task (snapshot or stream).
 *
 * Returns immediately: zero on success, or a negative code if already closed. Does NOT free
 * `user_data`; the `on_value` callback still fires once more with a terminal `0` (or a negative
 * error), which is where `user_data` should be released. Values already delivered remain valid
 * until released with [moq_consume_json_value_free].
 */
int32_t moq_consume_json_cancel(uint32_t task);

/**
 * Open an audio track on a broadcast.
 *
 * The encoder configuration is fixed at construction; subsequent
 * frame writes pass only payload + timestamp via
 * [`moq_encode_audio_frame`].
 *
 * Returns a non-zero handle on success or a negative error code.
 *
 * # Safety
 * - `name` must point to `name_len` bytes of UTF-8.
 * - `input` / `output` must point to fully populated structs.
 * - `output->codec` must point to `output->codec_len` bytes of UTF-8.
 * - `bandwidth` is a handle from [`crate::moq_session_bandwidth`], or 0 to leave the
 *   configured bitrate unclaimed.
 */
int32_t moq_encode_audio(uint32_t broadcast,
                         const char *name,
                         uintptr_t name_len,
                         const struct moq_audio_encoder_input *input,
                         const struct moq_audio_encoder_output *output,
                         uint32_t bandwidth);

/**
 * This encoder's bandwidth reservation, or 0 if it was published without one.
 *
 * Closing the returned handle does not release the encoder's claim; that lasts
 * until [moq_encode_audio_finish].
 */
int32_t moq_encode_audio_reservation(uint32_t producer);

/**
 * Watch whether the encoded audio track has subscribers, so the microphone and
 * encoder run only while someone listens. See [`crate::moq_publish_media_demand`]
 * for the callback contract.
 *
 * Returns a non-zero watcher handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_demand` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_demand` callback.
 */
int32_t moq_encode_audio_demand(uint32_t producer, moq_status_callback on_demand, void *user_data);

/**
 * Push one audio frame.
 *
 * `frame->data` is borrowed for the duration of the call; the
 * producer copies before returning.
 *
 * # Safety
 * - `frame` must point to a valid [`moq_audio_frame`].
 * - `frame->data` must point to `frame->data_size` bytes.
 */
int32_t moq_encode_audio_frame(uint32_t producer, const struct moq_audio_frame *frame);

/**
 * Flush any pending samples and finalize an audio producer.
 */
int32_t moq_encode_audio_finish(uint32_t producer);

/**
 * Subscribe to an audio track and decode it into PCM.
 *
 * The catalog `index` identifies which audio rendition to subscribe
 * to, matching the existing `moq_consume_audio` selection
 * model. TODO: a future API will pick the right rendition
 * automatically (ABR).
 *
 * Returns a non-zero handle on success or a negative error code.
 *
 * `on_frame` is called with a positive frame ID per frame, then exactly once
 * more with a terminal code: `0` (closed cleanly) or a negative error. After
 * the terminal (`<= 0`) callback, `on_frame` is never called again and
 * `user_data` is never touched again, so release `user_data` there. The
 * terminal callback fires even after [`moq_decode_audio_cancel`].
 *
 * Starts at the newest cached group so reopening live playback skips the backlog.
 *
 * A packet the codec cannot decode is logged and skipped rather than ending
 * the subscription, so a single bad frame costs that frame and not the stream.
 *
 * # Safety
 * - `output` must point to a valid [`moq_audio_decoder_output`].
 * - `user_data` must stay valid until the terminal (`<= 0`) `on_frame` callback.
 */
int32_t moq_decode_audio(uint32_t catalog,
                         uint32_t index,
                         const struct moq_audio_decoder_output *output,
                         moq_status_callback on_frame,
                         void *user_data);

/**
 * Stop an audio (raw PCM) consumer's background task.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`; the on-frame callback still fires once more with a
 * terminal `0` (or a negative error), which is where `user_data` should be
 * released. Frame IDs already delivered to the callback are likewise not freed;
 * release each with [`moq_decode_audio_frame_free`].
 */
int32_t moq_decode_audio_cancel(uint32_t consumer);

/**
 * Copy a delivered frame's metadata into `dst`.
 *
 * The written `dst->data` pointer remains valid until the same `id`
 * is released with [`moq_decode_audio_frame_free`].
 *
 * # Safety
 * - `dst` must point to a writable [`moq_audio_frame`].
 */
int32_t moq_decode_audio_frame(uint32_t id, struct moq_audio_frame *dst);

/**
 * Free a frame previously delivered through the consume callback.
 * Required for every delivered frame ID; closing the parent consumer
 * is not enough.
 */
int32_t moq_decode_audio_frame_free(uint32_t id);

/**
 * The session's bandwidth allocator. Clones share one reservation registry.
 *
 * Returns a non-zero handle, or a negative error if the session is unknown.
 */
int32_t moq_session_bandwidth(uint32_t session);

/**
 * Release a bandwidth handle. Reservations taken against it stay until they
 * themselves are closed (or the session's allocator is gone).
 */
int32_t moq_bandwidth_close(uint32_t bandwidth);

/**
 * Reserve up to `max_bps` for `track`, returning a reservation handle.
 *
 * `max_bps` is a ceiling, not a measurement: reserve the most the track can
 * ever send. Drop the reservation with [`moq_reservation_close`] to hand the
 * room back.
 */
int32_t moq_bandwidth_reserve(uint32_t bandwidth, uint32_t track, uint64_t max_bps);

/**
 * This reservation's slice right now, in bits per second.
 *
 * `present` is false when there is no estimate or no demand: hold the current
 * rate. `present` true and `bps` 0 is a real zero grant.
 *
 * # Safety
 * - `bps` and `present` must be valid pointers.
 */
int32_t moq_reservation_grant(uint32_t reservation, uint64_t *bps, bool *present);

/**
 * Change the ceiling, keeping the same claim.
 */
int32_t moq_reservation_update(uint32_t reservation, uint64_t max_bps);

/**
 * Release a reservation, handing its share back to siblings.
 *
 * Closing an accessor returned by a video or audio producer does not release
 * the encoder's claim; that lasts until the producer is finished.
 */
int32_t moq_reservation_close(uint32_t reservation);

/**
 * Open a video track on a broadcast, encoding the raw frames you publish to it.
 *
 * The encoder is opened here, so an unsupported codec, resolution, or backend
 * fails now rather than on the first frame. The track is named after the codec
 * (`.avc3` / `.hev1`) and its catalog rendition is published immediately, read
 * out of the encoder rather than guessed, so a subscriber can find the track
 * before a frame is written to it.
 *
 * Returns a non-zero handle on success or a negative error code.
 *
 * # Safety
 * - `input` / `output` must point to fully populated structs.
 * - `output->encoder` must point to `output->encoder_len` bytes of UTF-8 when
 *   `output->kind` is `MOQ_VIDEO_ENCODER_KIND_NAMED`.
 * - `bandwidth` is a handle from [`crate::moq_session_bandwidth`], or 0 to hold the
 *   configured bitrate regardless of congestion.
 */
int32_t moq_encode_video(uint32_t broadcast,
                         const struct moq_video_encoder_input *input,
                         const struct moq_video_encoder_output *output,
                         uint32_t bandwidth);

/**
 * This encoder's bandwidth reservation, or 0 if it was published without one.
 *
 * Closing the returned handle does not release the encoder's claim; that lasts
 * until [moq_encode_video_finish].
 */
int32_t moq_encode_video_reservation(uint32_t producer);

/**
 * Watch whether the encoded video track has subscribers, so the camera and encoder
 * run only while someone watches. See [`crate::moq_publish_media_demand`] for the
 * callback contract.
 *
 * Returns a non-zero watcher handle on success, or a negative code on failure.
 *
 * # Safety
 * - `on_demand` must be non-NULL.
 * - The caller must keep `user_data` valid until the terminal (`<= 0`) `on_demand` callback.
 */
int32_t moq_encode_video_demand(uint32_t producer, moq_status_callback on_demand, void *user_data);

/**
 * Encode and publish one raw frame.
 *
 * `frame->data` is borrowed for the duration of the call and must be exactly one
 * picture in the pixel format and at the resolution declared by
 * [`moq_video_encoder_input`].
 * A backend that pipelines publishes an earlier frame's output here, so a call
 * that emits nothing is normal rather than an error.
 *
 * # Safety
 * - `frame` must point to a valid [`moq_video_encoder_frame`].
 * - `frame->data` must point to `frame->data_size` bytes.
 */
int32_t moq_encode_video_frame(uint32_t producer, const struct moq_video_encoder_frame *frame);

/**
 * Cut a new group at the next published frame.
 *
 * Optional. The encoder already keyframes every `moq_video_encoder_output.gop`
 * frames, and each of those cuts a group, so a subscriber can always join
 * without you calling this. Reach for it only to place the boundaries yourself:
 * aligning groups with something the encoder cannot see, such as a scene change,
 * a source switch, or resuming after an idle gap.
 *
 * The next frame is encoded as a keyframe, which closes the open group and
 * starts a new one at it. Calling this repeatedly before that frame arrives cuts
 * once, not several times.
 *
 * Fails when the selected encoder cannot force a keyframe (a V4L2 driver
 * without the control): nothing is queued, and groups keep falling every
 * `gop` frames.
 */
int32_t moq_encode_video_cut(uint32_t producer);

/**
 * Retune a live encoder to `bitrate` bits per second, taking effect from
 * roughly the next frame. No keyframe is forced, so this is cheap enough to
 * drive from a congestion controller.
 *
 * The configured bitrate is a ceiling on some backends (openh264 rejects a raise
 * above the rate it opened at), so set `bitrate` to the highest you will ask
 * for and adapt downwards from there.
 *
 * When this encoder was published against a bandwidth allocator, the reservation
 * and follower ceiling move with it, so a later grant cannot retune above this
 * value.
 *
 * Returns a negative code if this backend cannot retune while running. That is
 * not fatal: the encoder keeps running at its current rate, so stop adapting
 * rather than stop publishing.
 */
int32_t moq_encode_video_bitrate(uint32_t producer, uint64_t bitrate);

/**
 * Flush any frames the codec is still holding and finalize the video track.
 *
 * The handle is released, so nothing can be published to it afterwards.
 */
int32_t moq_encode_video_finish(uint32_t producer);

/**
 * Subscribe to a video track and decode it into raw frames in the requested
 * CPU pixel format and size (see [`moq_video_decoder_output`]).
 *
 * The catalog `index` selects which video rendition to subscribe to, matching
 * the existing `moq_consume_video` selection model. Only H.264 is
 * supported; a non-H.264 rendition fails on the terminal callback.
 *
 * An unknown `output->format` or an invalid `output->width`/`height` fails
 * here, before subscribing: an accepted request always produces the requested
 * layout or fails on the terminal callback instead of delivering it silently.
 *
 * Returns a non-zero handle on success or a negative error code.
 *
 * `on_frame` is called with a positive frame id per decoded frame, then exactly
 * once more with a terminal code: `0` (closed cleanly) or a negative error.
 * After the terminal (`<= 0`) callback, `on_frame` is never called again and
 * `user_data` is never touched again, so release `user_data` there. The terminal
 * callback fires even after [`moq_decode_video_cancel`].
 *
 * Starts at the newest cached group so reopening live playback skips the backlog.
 *
 * # Safety
 * - `output` must point to a valid [`moq_video_decoder_output`].
 * - `user_data` must stay valid until the terminal (`<= 0`) `on_frame` callback.
 */
int32_t moq_decode_video(uint32_t catalog,
                         uint32_t index,
                         const struct moq_video_decoder_output *output,
                         moq_status_callback on_frame,
                         void *user_data);

/**
 * Stop a video (raw) consumer's background task.
 *
 * Returns immediately: zero on success, or a negative code if already closed.
 * Does NOT free `user_data`; the on-frame callback still fires once more with a
 * terminal `0` (or a negative error), which is where `user_data` should be
 * released. Frame ids already delivered are likewise not freed; release each
 * with [`moq_decode_video_frame_free`].
 */
int32_t moq_decode_video_cancel(uint32_t consumer);

/**
 * Copy a delivered frame's metadata into `dst`.
 *
 * The written `dst->data` pointer remains valid until the same `id` is released
 * with [`moq_decode_video_frame_free`].
 *
 * # Safety
 * - `dst` must point to a writable [`moq_video_frame`].
 */
int32_t moq_decode_video_frame(uint32_t id, struct moq_video_frame *dst);

/**
 * Free a frame previously delivered through the consume callback. Required for
 * every delivered frame id; closing the parent consumer is not enough.
 */
int32_t moq_decode_video_frame_free(uint32_t id);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
