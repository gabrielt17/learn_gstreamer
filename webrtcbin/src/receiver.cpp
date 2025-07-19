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

GstElement *webrtc_recv;

static void on_answer_created(GstPromise *promise, gpointer user_data) {
    GstWebRTCSessionDescription *answer = NULL;
    const GstStructure *reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, NULL);
    g_signal_emit_by_name(webrtc_recv, "set-local-description", answer, NULL);
    
    gchar *sdp_text = gst_sdp_message_as_text(answer->sdp);
    json sdp_answer = {{"type", "answer"}, {"sdp", sdp_text}};
    std::cout << "\n>>> RESPOSTA (copie e cole no remetente):\n" << sdp_answer.dump() << std::endl;

    g_free(sdp_text);
    gst_webrtc_session_description_free(answer);
}

static void on_ice_candidate(GstElement *webrtc, guint mline_index, gchar *candidate) {
    json ice_candidate = {
        {"type", "candidate"},
        {"candidate", candidate},
        {"sdpMLineIndex", mline_index}
    };
    std::cout << "\n>>> CANDIDATO ICE (copie e cole no remetente):\n" << ice_candidate.dump() << std::endl;
}

static void on_pad_added(GstElement *webrtc, GstPad *pad, GstElement *pipeline) {
    GstElement *depay = gst_bin_get_by_name(GST_BIN(pipeline), "depay");
    GstPad *sinkpad = gst_element_get_static_pad(depay, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
    gst_object_unref(depay);
}

static gboolean handle_stdin(GIOChannel *source, GIOCondition condition, gpointer user_data) {
    gchar *line_str = NULL;
    g_io_channel_read_line(source, &line_str, NULL, NULL, NULL);

    try {
        json msg = json::parse(line_str);
        if (msg["type"] == "offer") {
            std::string sdp_text = msg["sdp"];
            g_print("--> Recebida Oferta SDP, criando resposta...\n");
            GstSDPMessage *sdp = NULL;
            gst_sdp_message_new_from_text(sdp_text.c_str(), &sdp);
            GstWebRTCSessionDescription *offer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_OFFER, sdp);
            g_signal_emit_by_name(webrtc_recv, "set-remote-description", offer, NULL);

            GstPromise *promise = gst_promise_new_with_change_func(on_answer_created, NULL, NULL);
            g_signal_emit_by_name(webrtc_recv, "create-answer", NULL, promise);
            gst_webrtc_session_description_free(offer);
        } else if (msg["type"] == "candidate") {
            std::string candidate = msg["candidate"];
            guint mline_index = msg["sdpMLineIndex"];
            g_print("--> Recebido Candidato ICE, adicionando...\n");
            g_signal_emit_by_name(webrtc_recv, "add-ice-candidate", mline_index, candidate.c_str());
        }
    } catch (json::parse_error& e) {}
    
    g_free(line_str);
    return TRUE;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    GstElement *pipeline = gst_parse_launch("webrtcbin name=recv ! rtpvp8depay name=depay ! vp8dec ! videoconvert ! xvimagesink", NULL);
    webrtc_recv = gst_bin_get_by_name(GST_BIN(pipeline), "recv");

    g_signal_connect(webrtc_recv, "on-ice-candidate", G_CALLBACK(on_ice_candidate), NULL);
    g_signal_connect(webrtc_recv, "pad-added", G_CALLBACK(on_pad_added), pipeline);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GIOChannel *io_stdin = g_io_channel_unix_new(fileno(stdin));
    g_io_add_watch(io_stdin, G_IO_IN, handle_stdin, NULL);
    g_print("Pipeline do receptor iniciada. Cole a Oferta SDP (JSON) do remetente abaixo:\n");
    
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}