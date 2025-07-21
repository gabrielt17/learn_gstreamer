// src/sender.cpp
#define GST_USE_UNSTABLE_API

#include <cstdio>
#include <iostream>
#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>
#include <glib.h>
#include <nlohmann/json.hpp>
#include <ixwebsocket/IXWebSocket.h>

using json = nlohmann::json;

GstElement *webrtc_send;

static void on_offer_created(GstPromise *promise, gpointer user_data) {
    GstWebRTCSessionDescription *offer = NULL;
    const GstStructure *reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer, NULL);
    g_signal_emit_by_name(webrtc_send, "set-local-description", offer, NULL);

    gchar *sdp_text = gst_sdp_message_as_text(offer->sdp);
    json sdp_offer = {{"type", "offer"}, {"sdp", sdp_text}};
    std::cout << "\n>>> OFERTA (copie e cole no receptor):\n" << sdp_offer.dump() << std::endl;

    g_free(sdp_text);
    gst_webrtc_session_description_free(offer);
}

static void on_negotiation_needed(GstElement *element, gpointer user_data) {
    g_print("--> Negociação necessária, criando oferta...\n");
    GstPromise *promise = gst_promise_new_with_change_func(on_offer_created, NULL, NULL);
    g_signal_emit_by_name(element, "create-offer", NULL, promise);
}

static void on_ice_candidate(GstElement *webrtc, guint mline_index, gchar *candidate) {
    json ice_candidate = {
        {"type", "candidate"},
        {"candidate", candidate},
        {"sdpMLineIndex", mline_index}
    };
    std::cout << "\n>>> CANDIDATO ICE (copie e cole no receptor):\n" << ice_candidate.dump() << std::endl;
}

static gboolean handle_stdin(GIOChannel *source, GIOCondition condition, gpointer user_data) {
    gchar *line_str = NULL;
    g_io_channel_read_line(source, &line_str, NULL, NULL, NULL);
    
    try {
        json msg = json::parse(line_str);
        if (msg["type"] == "answer") {
            std::string sdp_text = msg["sdp"];
            g_print("--> Recebida Resposta SDP, configurando...\n");
            GstSDPMessage *sdp = NULL;
            gst_sdp_message_new_from_text(sdp_text.c_str(), &sdp);
            GstWebRTCSessionDescription *answer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_ANSWER, sdp);
            g_signal_emit_by_name(webrtc_send, "set-remote-description", answer, NULL);
            gst_webrtc_session_description_free(answer);
        } else if (msg["type"] == "candidate") {
            std::string candidate = msg["candidate"];
            guint mline_index = msg["sdpMLineIndex"];
            g_print("--> Recebido Candidato ICE, adicionando...\n");
            g_signal_emit_by_name(webrtc_send, "add-ice-candidate", mline_index, candidate.c_str());
        }
    } catch (json::parse_error& e) {}

    g_free(line_str);
    return TRUE;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    GstElement *pipeline = gst_parse_launch("videotestsrc is-live=true ! videoconvert ! vp8enc deadline=1 ! rtpvp8pay ! webrtcbin name=send", NULL);
    webrtc_send = gst_bin_get_by_name(GST_BIN(pipeline), "send");

    g_signal_connect(webrtc_send, "on-negotiation-needed", G_CALLBACK(on_negotiation_needed), NULL);
    g_signal_connect(webrtc_send, "on-ice-candidate", G_CALLBACK(on_ice_candidate), NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GIOChannel *io_stdin = g_io_channel_unix_new(fileno(stdin));
    g_io_add_watch(io_stdin, G_IO_IN, handle_stdin, NULL);
    
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    
    return 0;
}