#include <gst/gst.h>

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);

    GstElement *pipeline;

    pipeline = gst_element_factory_make("playbin", "player");

    if (!pipeline)
    {

        g_printerr("Erro ao criar a pipeline.\n");
        return -1;
    }

    g_object_set(pipeline, "uri", "file:///workspaces/learn_gstreamer/carregar_video/walk_pretender.mp4", NULL);

    GstStateChangeReturn ret;

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {

        g_printerr("Deu pau quando tentei dar play.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    GstBus *bus;
    GstMessage *msg;
    gboolean terminate = FALSE;

    bus = gst_element_get_bus(pipeline);

    do
    {
        msg = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);

        if (msg != NULL)
        {
            GError *err;
            gchar *debuginfo;

            switch (GST_MESSAGE_TYPE(msg))
            {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debuginfo);
                g_printerr("Erro recebido %s: %s\n",
                           GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Informações de debug: %s",
                           debuginfo ? debuginfo : "nada");
                g_clear_error(&err);
                g_free(debuginfo);
                terminate = TRUE;
                break;
            case GST_MESSAGE_EOS:
                gst_print("Terminou o vídeo");
                terminate = TRUE;
                break;
            default:
                g_print("Alguma mensagem desconhecida foi gerada.\n");
                break;
            }
            gst_message_unref(msg);
        }
    } while (!terminate);

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}