// src/receiver.cpp
#define GST_USE_UNSTABLE_API

#include <cstdio>
#include <iostream>
#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>
#include <glib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

GstElement *webrtc_recv = nullptr;

static void on_answer_created(GstPromise *promise, gpointer) {
    const GstStructure *reply = gst_promise_get_reply(promise);
    GstWebRTCSessionDescription *answer = nullptr;

    gst_structure_get(reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, nullptr);
    g_signal_emit_by_name(webrtc_recv, "set-local-description", answer, nullptr);

    gchar *sdp_text = gst_sdp_message_as_text(answer->sdp);
    json sdp_answer = {{"type", "answer"}, {"sdp", sdp_text}};
    std::cout << "\n>>> RESPOSTA (copie e cole no remetente):\n" << sdp_answer.dump() << std::endl;

    g_free(sdp_text);
    gst_webrtc_session_description_free(answer);
    gst_promise_unref(promise);
}

static void on_ice_candidate(GstElement *, guint mline_index, gchar *candidate) {
    json ice_candidate = {
        {"type", "candidate"},
        {"candidate", candidate},
        {"sdpMLineIndex", mline_index}
    };
    std::cout << "\n>>> CANDIDATO ICE (copie e cole no remetente):\n" << ice_candidate.dump() << std::endl;
}

static void on_pad_added(GstElement *, GstPad *pad, GstElement *pipeline) {
    GstElement *depay = gst_bin_get_by_name(GST_BIN(pipeline), "depay");
    GstPad *sinkpad = gst_element_get_static_pad(depay, "sink");
    if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
        g_printerr("Falha ao linkar pad.\n");
    }
    gst_object_unref(sinkpad);
    gst_object_unref(depay);
}

static gboolean handle_stdin(GIOChannel *source, GIOCondition, gpointer) {
    gchar *line_str = nullptr;
    g_io_channel_read_line(source, &line_str, nullptr, nullptr, nullptr);

    if (!line_str || g_strstrip(line_str)[0] == '\0') {
        g_free(line_str);
        return TRUE;
    }

    try {
        json msg = json::parse(line_str);
        if (msg["type"] == "offer") {
            std::string sdp_text = msg["sdp"];
            g_print("--> Recebida Oferta SDP, criando resposta...\n");
            GstSDPMessage *sdp = nullptr;
            if (gst_sdp_message_new_from_text(sdp_text.c_str(), &sdp) == GST_SDP_OK) {
                GstWebRTCSessionDescription *offer =
                    gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_OFFER, sdp);
                g_signal_emit_by_name(webrtc_recv, "set-remote-description", offer, nullptr);

                GstPromise *promise = gst_promise_new_with_change_func(on_answer_created, nullptr, nullptr);
                g_signal_emit_by_name(webrtc_recv, "create-answer", nullptr, promise);

                gst_webrtc_session_description_free(offer);
            } else {
                g_printerr("Falha ao analisar SDP da offer.\n");
            }
        } else if (msg["type"] == "candidate") {
            std::string candidate = msg["candidate"];
            guint mline_index = msg["sdpMLineIndex"];
            g_print("--> Recebido Candidato ICE, adicionando...\n");
            g_signal_emit_by_name(webrtc_recv, "add-ice-candidate", mline_index, candidate.c_str());
        }
    } catch (json::parse_error &) {
        g_printerr("Entrada inválida: não é JSON.\n");
    }

    g_free(line_str);
    return TRUE;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GError *err = nullptr;
    GstElement *pipeline = gst_parse_launch(
        "webrtcbin name=recv ! rtpvp8depay name=depay ! vp8dec ! videoconvert ! autovideosink",
        &err
    );

    if (!pipeline) {
        g_printerr("Falha ao criar pipeline: %s\n", err->message);
        g_clear_error(&err);
        return -1;
    }

    webrtc_recv = gst_bin_get_by_name(GST_BIN(pipeline), "recv");

    g_signal_connect(webrtc_recv, "on-ice-candidate", G_CALLBACK(on_ice_candidate), nullptr);
    g_signal_connect(webrtc_recv, "pad-added", G_CALLBACK(on_pad_added), pipeline);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GIOChannel *io_stdin = g_io_channel_unix_new(fileno(stdin));
    g_io_add_watch(io_stdin, G_IO_IN, handle_stdin, nullptr);

    g_print("Pipeline do receptor iniciada. Cole a Oferta SDP (JSON) do remetente abaixo:\n");

    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);

    // Limpeza
    g_main_loop_unref(loop);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(webrtc_recv);
    gst_object_unref(pipeline);
    g_io_channel_unref(io_stdin);

    return 0;
}
