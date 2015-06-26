# KafkaLite

## Installation

## Running tests

## Basic Usage

1. include header:

	```
	#include <kafkalite.h>
	```

2. Create a context with a base folder:

	```
	KLContext *context = kl_context_new("/tmp/k1", NULL);
	```

	A KLContext manages one or topics into which messages can be published into
	and consumed from.  A KLContext can be thought of as a topic group where
	messages being written is serialized.   This allows the client to configure
	the level of concurrent writes to a set of topics (to increase parallel
	publishes, new contexts can simply be created).   Messages can be consumed
	concurrently.

3. Create topics to which to publish messages.  This can be done at any time.
   Duplicate calls to opening a topic would return the original instance:

	```
	KLTopic *topic1 = kl_topic_open(context, "mytopic");
	```

4. When finished with the topic call:

	```
	kl_topic_close(topic1)
	```

5. When finished with the context call:

	```
	kl_context_destroy(context)
	```

