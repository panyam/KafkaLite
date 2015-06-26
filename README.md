# KafkaLite

## Installation

Coming soon - packaging and brewing it up!

## Basic Usage

1. include header:

	```
	#include <kafkalite.h>
	```
	
2. Create a context with a base folder:

	```
	KLContext *context = kl_context_new("/tmp/kafka/", NULL);
	```

	A KLContext manages one or topics into which messages can be published into
	and consumed from.  A KLContext can be thought of as a topic group where
	messages being written is serialized.   This allows the client to configure
	the level of concurrent writes to a set of topics (to increase parallel
	publishes, new contexts can simply be created).   Messages can be consumed
	concurrently.

3. Create topics to publish messages into

	```
	KLTopic *topic1 = kl_topic_open(context, "mytopic");
	```

	Each topic is a single queue into which messages are written in the order in 
	which they are enqueued.  Topics can be opened and closed at any time concurrently.  
	Duplicate calls to opening a topic would return the original instance.  Internally 
	a reference count is maintained so that topics are only deallocated when all 
	owners of a topic call the corresponding kl_topic_close on a topic.

4. When finished with the topic call:

	```
	kl_topic_close(topic1)
	```

	Closes a topic and relinquishes ownership of the topic by the calling owner.  
	Using this topic after a close will result in possible exceptions due to the
	possibility of it being deallocated.  Internally the topic's reference count is
	decremented (and deallocated if necessary).

5. Publish messages to a topic:

	```
	char *message = "hello world";
	kl_topic_publish(topic1, message, strlen(message) + 1 /* + 1 for the null char */);
	```
	
	This appends the message to the topic.  The messages are very simple and it does not 
	matter what the content's format or structure is.  (*TODO*: Should a messageType (int)
	also be allowed in the publishing to allow easy and simple filtering during consumption?).
	
6. Verify and/or Consume messages

	Message consumption from a topic happens via iterators.  A topic can have multiple consumers.
	Each consumer is provided an iterator interface that can start at any point in the topic
	and messages are consumed in the order in which they were published.   To consume messages:

	```	
		KLIterator *iterator = kl_iterator_new(context, "mytopic", 0);
		bool hasMore = kl_iterator_forward(iterator);
		if (hasMore)
		{
		        size_t msgsize = kl_iterator_msgsize(iterator);
	        	KLMessage *message = (KLMessage *)malloc(sizeof(KLMessage) + msgsize);
	        	kl_iterator_message(iterator, message);
	        
	        	// Do something in the message...
			printf("Message: %s\n", message->data);
        	}
	```

	Multiple iterators can be created to start at different offsets and can iterate
	and consume messages concurrently.
	
7. When finished with the context call:
	```
	kl_context_destroy(context)
	```

	This closes the context and all associated topics.   Any handle to this
	context or topics assigned within this context (with kl_topic_open) is now
	unusable.
