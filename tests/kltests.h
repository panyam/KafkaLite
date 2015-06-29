
#ifndef __KL_TESTS_BASE_H__
#define __KL_TESTS_BASE_H__

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <netinet/in.h>
#include <kafkalite.h>

using namespace CppUnit;
using namespace std;

#define TEST_DIR	"/tmp/kafka"

// Generate random messages
extern const char *SAMPLE_MESSAGE;
extern const UInt64 SAMPLE_MESSAGE_LEN;

extern void makeRandomMessage(char *buffer, int msgId, int nRand);
extern long long publishMessages(KLTopic *topic, int numMessages, int nRandMax, bool assertOffsets);

class KLBaseTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KLBaseTest);
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp(void) { }
	virtual void tearDown(void) { }
};

#endif
