
#include "kltests.h"
#include <assert.h>

const long NUM_MESSAGES = 1000;
const long NUM_RAND_MAX = 100;

int main()
{
    kl_rmdirs(TEST_DIR);
    KLContext *context = kl_context_open(TEST_DIR, NULL);
    KLTopic *topic = kl_topic_open(context, "topic");
    publishMessages(topic, NUM_MESSAGES, NUM_RAND_MAX, false);
    KLIterator *iterator = kl_iterator_new(context, "topic", 0);
    assert(iterator != NULL);

    char buffer[4096];

    for (int i = 0;i < NUM_MESSAGES;i++)
    {
        bool result = kl_iterator_forward(iterator);

        // get the message size
        size_t msgsize = kl_iterator_msgsize(iterator);
        KLMessage *message = (KLMessage *)malloc(sizeof(KLMessage) + msgsize + 1);
        kl_iterator_message(iterator, message);
        message->data[msgsize] = 0;
        makeRandomMessage(buffer, i, i % NUM_RAND_MAX);
        KLMessageHeader messageInfo = kl_iterator_metadata(iterator);
        if (strlen(buffer) != msgsize || message->size != msgsize || strncmp(buffer, message->data, msgsize) != 0)
        {
            kl_log("\nFailed at I: %d, Offset: %ld, Size: %lu", i, messageInfo.offset, messageInfo.size);
            kl_log("\nBuffer:  |%s|", buffer);
            kl_log("\nMessage: |%s|", message->data);
        }
        assert(strlen(buffer) == msgsize);
        assert(message->size == msgsize);
        assert(strncmp(buffer, message->data, msgsize) == 0);
        free(message);
    }
    return 0;
}
