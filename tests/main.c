
#include "KLContextTests.h"
#include "KLTopicTests.h"

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;
 
    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);
 
    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);
 
    // insert test-suite at test-runner by registry
	bool alltests = false;
	if (alltests)
	{
    	CPPUNIT_NS::TestRunner testrunner;
    	testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    	testrunner.run(testresult);
	} else {
		CppUnit::TestCaller<KLTopicTests> test("test_kl_topic_restart", &KLTopicTests::test_kl_topic_restart);
		test.run(&testresult);
	}
 
    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();
 
    // Output XML for Jenkins CPPunit plugin
    // ofstream xmlFileOut("cppTestBasicMathResults.xml");
    // XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    // xmlOut.write();
 
    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
