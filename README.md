# KafkaLite

## Installation

### From sources

* Checkout from github
* prepare, configure and install:

```
sh prepare.sh
./configure
make
make install
```

Automake version 1.14 or above is required.  This is available on homebrew.

### From a particular release

* Download sources for a particular version from: https://github.com/panyam/KafkaLite/releases
* configure and install:

```
tar -zxvf libkafkalite-<version>.tar.gz
./configure
make
make install
```

Automake version 1.14 or above is required.  This is available on homebrew.

### For an iOS project

Add the KafkaLite pod to your Podfile:

```
pod 'KafkaLite'
```

or to get the latest development version:

```
pod 'KafkaLite', :git => 'https://github.com/panyam/KafkaLite.git'
```


## Basic Usage

1. include header:

    ```
    #include <kafkalite.h>
    ```
    
2. Create a context with a base folder:

    ```
    KLContext *context = kl_context_open("/tmp/kafka/", NULL);
    ```

    A KLContext manages one or more topics into which messages can be published into
    and consumed from.  A KLContext can be thought of as a topic group where
    messages being written are serialized.   This allows the client to configure
    the level of concurrent writes to a set of topics (to increase parallel
    publishes, new contexts can simply be created).   Messages can be consumed
    concurrently.
    
    The second parameter accepts the factory/manager for managing locks.  NULL implies
    no locking is required (as access to KL is already synchronized).   Alternatively 
    ```kl_pthread_lock_manager()``` can be passed as a parameter to use locks based on
    the pthreads library.  

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
    also be allowed in the publishing to allow easy and simple filtering during consumption 
    without incurring a marshalling cost?).
    
6. Verify and/or Consume messages

    Message consumption from a topic happens via iterators.  A topic can have multiple consumers.
    Each consumer is provided an iterator interface that can start at any point in the topic
    and messages are consumed in the order in which they were published.   To consume messages:

    ```    
        KLIterator *iterator = kl_iterator_new(context, "mytopic", 0);
        bool hasMore = kl_iterator_forward(iterator);
        if (hasMore)
        {
            uint64_t msgsize = kl_iterator_msgsize(iterator);
            KLMessage *message = (KLMessage *)malloc(sizeof(KLMessage) + msgsize);
            kl_iterator_message(iterator, message);
            
            // Do something in the message...
            printf("Message: %s\n", message->data);
        }
    ```

    Multiple iterators can be created to start at different offsets and can iterate
    and consume messages concurrently.

    After an iterator has been used, it can be destroyed with:
    ```
    kl_iterator_destroy(context)
    ```

7. When finished with the context call:
    ```
    kl_context_destroy(context)
    ```

    This closes the context and all associated topics.   Any handle to this
    context or topics assigned within this context (with kl_topic_open) is now
    unusable.

## Benchmarks

Upon installation, a benchmark tool is created - klbench.  This allows to run the kafkalite library under several scenarios.  The following usage options are provided (by running klbench -h):

```
Usage: /home/panyam/projects/KafkaLite/benchmarks/.libs/lt-klbench <options>
    Options:
        -m    Number of messages to publish
        -p    Number of producers
        -c    Number of consumers
        -nt   Number of threads.  1 Thread will be for publisher and the rest of the threads will be for consumers
        -t    Comma separated names of test message files that will be published randomly
        -d    Size of payload to publish.  This is ignored if test files are specified via 
              the -t option.  If neither -t or -d are specified then -d is implied with a 
              default message size of 256 bytes
        -l    The number of messages by which the consumer will lag the publisher.
```

Following benchmarks have been run:

1. Single Producer / Single Consumer (with a variable lag) in a single thread
2. Single Producer and Multiple consumer all in a single thread with variable lag between each actor.
3. Single Producer and Single Consumer running in two different threads.
4. Single Producer and Multiple Consumers all in different threads.
5. Single Producer (1 thread) and Multiple Consumers (1 thread for all Consumers)
 

## Things to do

1. Add more benchmarks.
2. Make file and thread APIs more abstract to enable use of native constructs
   (such as WinThreads, GCD etc).
3. Focus on improving flush times on lower memory usage.
4. Block consumers when at the end of a log and no messages are available (this can be implemented at a layer above for now).
5. Current message headers are very simple.   May be allow an option to allow the user to specify header size to allow arbitraty data to be passed in the header in users' discretion.  The header size would still be fixed on a pe topic basis but this allows the user to tradeoff header size with message richness so more can be done at a message header level (eg filtering etc) without reading the entire message.
