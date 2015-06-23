# KafkaLite

Usage:

1. include header:

```
#include <kafkalite.h>
```

2. create a context with base folder to store all queued messages:

```
KLContext *context = kl_context_new("/tmp/k1");
```

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
