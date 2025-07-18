// Adicione esta linha NO TOPO do arquivo, antes de qualquer include.
#define GST_USE_UNSTABLE_API

#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>

// Ponteiros globais com nomes corrigidos
GstElement *pipe1, *pipeline2, *webrtc1, *webrtc2;

// Protótipos das funções
static void on_negotiation_needed(GstElement *element, gpointer user_data);
static void on_ice_candidate(GstElement *webrtc, guint mline_index, gchar *candidate, gpointer user_data);
static void on_pad_added(GstElement *webrtc, GstPad *pad, GstElement *pipe);

// Callback para a criação da oferta
static void on_offer_created(GstPromise *promise, gpointer user_data) {
    g_print("--> OFERTA criada por webrtc1, configurando descrições...\n");
    GstWebRTCSessionDescription *offer = NULL;
    gst_promise_wait(promise);
    const GstStructure *reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "offer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer, NULL);
    g_signal_emit_by_name(webrtc1, "set-local-description", offer, NULL);
    g_signal_emit_by_name(webrtc2, "set-remote-description", offer, NULL);
    gst_webrtc_session_description_free(offer);
}

// Callback para a criação da resposta
static void on_answer_created(GstPromise *promise, gpointer user_data) {
    g_print("--> RESPOSTA criada por webrtc2, configurando descrições...\n");
    GstWebRTCSessionDescription *answer = NULL;
    gst_promise_wait(promise);
    const GstStructure *reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, NULL);
    g_signal_emit_by_name(webrtc2, "set-local-description", answer, NULL);
    g_signal_emit_by_name(webrtc1, "set-remote-description", answer, NULL);
    gst_webrtc_session_description_free(answer);
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // --- Criação das Pipelines com Verificação ---

    g_print("Criando pipeline do remetente (pipe1)...\n");
    pipe1 = gst_parse_launch("videotestsrc ! videoconvert ! vp8enc deadline=1 ! rtpvp8pay ! webrtcbin name=send", NULL);
    if (!pipe1) {
        g_printerr("ERRO: Falha ao criar a pipeline do remetente.\n");
        return -1;
    }

    g_print("Criando pipeline do receptor (pipeline2)...\n");
    pipeline2 = gst_parse_launch("webrtcbin name=recv ! rtpvp8depay name=depay ! vp8dec ! videoconvert ! autovideosink", NULL);
    if (!pipeline2) {
        g_printerr("ERRO: Falha ao criar a pipeline do receptor.\n");
        return -1;
    }
    
    // --- Extração dos Elementos webrtcbin com Verificação ---

    g_print("Procurando por elementos 'send' e 'recv'...\n");
    webrtc1 = gst_bin_get_by_name(GST_BIN(pipe1), "send");
    if (!webrtc1) {
        g_printerr("ERRO: Elemento 'send' (webrtcbin) não encontrado na pipeline 1.\n");
        return -1;
    }

    webrtc2 = gst_bin_get_by_name(GST_BIN(pipeline2), "recv");
    if (!webrtc2) {
        g_printerr("ERRO: Elemento 'recv' (webrtcbin) não encontrado na pipeline 2.\n");
        return -1;
    }

    // --- Configuração dos Sinais (agora com ponteiros válidos) ---

    g_print("Configurando os sinais WebRTC...\n");
    g_signal_connect(webrtc1, "on-negotiation-needed", G_CALLBACK(on_negotiation_needed), NULL);
    g_signal_connect(webrtc2, "on-negotiation-needed", G_CALLBACK(on_negotiation_needed), NULL);
    g_signal_connect(webrtc1, "on-ice-candidate", G_CALLBACK(on_ice_candidate), webrtc2);
    g_signal_connect(webrtc2, "on-ice-candidate", G_CALLBACK(on_ice_candidate), webrtc1);
    g_signal_connect(webrtc2, "pad-added", G_CALLBACK(on_pad_added), pipeline2);
    
    g_print("Iniciando pipelines... A negociação WebRTC começará automaticamente.\n");
    gst_element_set_state(pipe1, GST_STATE_PLAYING);
    gst_element_set_state(pipeline2, GST_STATE_PLAYING);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Limpeza (continua igual)
    gst_element_set_state(pipe1, GST_STATE_NULL);
    gst_element_set_state(pipeline2, GST_STATE_NULL);
    gst_object_unref(webrtc1);
    gst_object_unref(webrtc2);
    gst_object_unref(pipe1);
    gst_object_unref(pipeline2);
    g_main_loop_unref(loop);

    return 0;
}

// Implementação dos callbacks (continua igual, mas agora usando 'pipeline2' onde for preciso)
void on_pad_added(GstElement *webrtc, GstPad *pad, GstElement *pipe) {
    g_print("--> Pad de mídia recebido no webrtc2, conectando ao decodificador...\n");
    GstElement *depay = gst_bin_get_by_name(GST_BIN(pipe), "depay");
    GstPad *sinkpad = gst_element_get_static_pad(depay, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}

void on_negotiation_needed(GstElement *element, gpointer user_data) {
    g_print("--> Negociação necessária para %s\n", GST_OBJECT_NAME(element));
    if (element == webrtc1) {
        GstPromise *promise = gst_promise_new_with_change_func(on_offer_created, NULL, NULL);
        g_signal_emit_by_name(webrtc1, "create-offer", NULL, promise);
    } else if (element == webrtc2) {
        GstPromise *promise = gst_promise_new_with_change_func(on_answer_created, NULL, NULL);
        g_signal_emit_by_name(webrtc2, "create-answer", NULL, promise);
    }
}

void on_ice_candidate(GstElement *webrtc, guint mline_index, gchar *candidate, gpointer user_data) {
    GstElement *other_webrtc = (GstElement *)user_data;
    g_print("--> Candidato ICE gerado por %s, adicionando em %s\n", GST_OBJECT_NAME(webrtc), GST_OBJECT_NAME(other_webrtc));
    g_signal_emit_by_name(other_webrtc, "add-ice-candidate", mline_index, candidate);
}