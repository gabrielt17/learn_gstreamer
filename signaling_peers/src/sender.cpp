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

// Variáveis globais
std::string ipAddress = "localhost";
std::string port = "8000";
std::string localId = "gabrielt"; // ID local do remetente
std::string remoteId = "alice"; // ID remoto do destinatário

// Objeto principal do WebRTC
GstElement *webrtc_send;

// Objeto WebSocket para comunicação
ix::WebSocket ws;

static void on_offer_created(GstPromise *promise, gpointer user_data) {

    // Quando a promessa for cumprida, extraímos a oferta SDP
    GstWebRTCSessionDescription *offer = NULL;
    const GstStructure *reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer, NULL);

    // Adicionamos a oferta SDP ao webrtc_send
    g_signal_emit_by_name(webrtc_send, "set-local-description", offer, NULL);

    // Enviamos a oferta SDP como uma mensagem JSON
    gchar *sdp_text = gst_sdp_message_as_text(offer->sdp);
    json sdp_offer = {
        {"id", remoteId},
        {"type", "offer"}, 
        {"sdp", sdp_text}
    };
    std::cout << "\n>>> OFERTA GERADA:\n" << sdp_offer.dump() << std::endl;
    ws.send(sdp_offer.dump());

    // Libera a memória
    g_free(sdp_text);
    gst_webrtc_session_description_free(offer);
    gst_promise_unref(promise);
}

static void on_negotiation_needed(GstElement *element, gpointer user_data) {

    g_print("--> Negociação necessária, criando oferta...\n");

    // Quando a promessa for cumprida, chamará on_offer_created
    GstPromise *promise = gst_promise_new_with_change_func(on_offer_created, NULL, NULL);

    // Solicita a criação de uma oferta
    g_signal_emit_by_name(element, "create-offer", NULL, promise);
}

static void on_ice_candidate(GstElement *webrtc, guint mline_index, gchar *candidate) {

    // Quando um candidato ICE é gerado, o enviamos como uma mensagem JSON
    json ice_candidate = {
        {"id", remoteId},
        {"type", "candidate"},
        {"candidate", candidate},
        {"sdpMLineIndex", mline_index}
    };
    ws.send(ice_candidate.dump());
    std::cout << "\n>>> CANDIDATO ICE GERADO:\n" << ice_candidate.dump() << std::endl;
}

// Callback para lidar com entradas via terminal, quando ela era utilizada
static gboolean handle_stdin(GIOChannel *source, GIOCondition condition, gpointer user_data) {
    gchar *line_str = NULL;

    // Pega a source (entrada padrão) e guarda a linha lida em line_str
    g_io_channel_read_line(source, &line_str, NULL, NULL, NULL);
    
    try {

        // Trata o json recebido
        json msg = json::parse(line_str);

        if (msg["type"] == "answer") {

            // Se for uma resposta, pega a SDP e adiciona como resposta remota
            std::string sdp_text = msg["sdp"];
            g_print("--> Recebida Resposta SDP, configurando...\n");
            GstSDPMessage *sdp = NULL;
            gst_sdp_message_new_from_text(sdp_text.c_str(), &sdp);
            GstWebRTCSessionDescription *answer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_ANSWER, sdp);
            g_signal_emit_by_name(webrtc_send, "set-remote-description", answer, NULL);

            // Libera a memória
            gst_webrtc_session_description_free(answer);

        } else if (msg["type"] == "candidate") {

            // Se for um candidato ICE, pega a string do candidato e o índice mline
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

    // Inicializa o GStreamer
    gst_init(&argc, &argv);

    // Constrói a pipeline do GStreamer
    GstElement *pipeline = gst_parse_launch(
        "autovideosrc ! videoconvert ! vp8enc deadline=1 ! rtpvp8pay ! webrtcbin name=send",
        NULL);

    // Isola o elemento webrtcbin em um objeto global
    webrtc_send = gst_bin_get_by_name(GST_BIN(pipeline), "send");

    // Associa os sinas a funções de callback
    g_signal_connect(webrtc_send, "on-negotiation-needed", G_CALLBACK(on_negotiation_needed), NULL);
    g_signal_connect(webrtc_send, "on-ice-candidate", G_CALLBACK(on_ice_candidate), NULL);

    // Conecta ao servidor de sinalização
    std::string signaling_url = "ws://" + ipAddress + ":" + port + "/" + localId; // URL do servidor de sinalização
    ws.setUrl(signaling_url);

    // Define o callback para mensagens recebidas
    ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            try {
                // Tenta analisar a mensagem JSON
                json message = json::parse(msg->str);

                // Verifica o tipo de mensagem
                if (message["type"] == "answer") {

                    // Pega a SDP e guarda como resposta
                    std::string sdp_text = message["sdp"];
                    g_print("--> Recebida resposta SDP, adicionando SDP remoto.\n");
                    GstSDPMessage *sdp = NULL;
                    gst_sdp_message_new_from_text(sdp_text.c_str(), &sdp);

                    // Adiciona a SDP como resposta
                    GstWebRTCSessionDescription *answer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_ANSWER, sdp);
                    g_signal_emit_by_name(webrtc_send, "set-remote-description", answer, NULL);
                    g_print("--> SDP remoto configurado.\n");

                    // Libera a memória
                    gst_webrtc_session_description_free(answer);

                } else if (message["type"] == "candidate") {

                    // Pega a string do cadidato e o índice mline
                    std::string candidate = message["candidate"];
                    guint mline_index = message["sdpMLineIndex"];

                    // Adiciona o candidato ICE
                    g_print("--> Recebido Candidato ICE, adicionando...\n");
                    g_signal_emit_by_name(webrtc_send, "add-ice-candidate", mline_index, candidate.c_str());

                } else {
                    g_printerr("-!>Tipo de mensagem desconhecido: %s\n", message["type"].get<std::string>().c_str());
                }
            } catch (json::parse_error &) {
                g_printerr("-!> Entrada inválida: não é JSON.\n");
            }
        }
    });

    // Inicia a conexão WebSocket
    ws.start();
    
    // Aguarda até que a conexão WebSocket esteja aberta
    while (ws.getReadyState() != ix::ReadyState::Open) {
        g_print("--> Aguardando conexão WebSocket...\n");
        g_usleep(100000); // Espera até que a conexão esteja aberta
    }

    // Inicia a pipeline
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Monitora a entrada padrão para receber ofertas e candidatos ICE
    GIOChannel *io_stdin = g_io_channel_unix_new(fileno(stdin));
    g_io_add_watch(io_stdin, G_IO_IN, handle_stdin, NULL);
    
    // Inicia o loop principal do GStreamer
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    
    return 0;
}