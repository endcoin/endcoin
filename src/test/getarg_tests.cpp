#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-END");
    BOOST_CHECK(GetBoolArg("-END"));
    BOOST_CHECK(GetBoolArg("-END", false));
    BOOST_CHECK(GetBoolArg("-END", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ENDo"));
    BOOST_CHECK(!GetBoolArg("-ENDo", false));
    BOOST_CHECK(GetBoolArg("-ENDo", true));

    ResetArgs("-END=0");
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", false));
    BOOST_CHECK(!GetBoolArg("-END", true));

    ResetArgs("-END=1");
    BOOST_CHECK(GetBoolArg("-END"));
    BOOST_CHECK(GetBoolArg("-END", false));
    BOOST_CHECK(GetBoolArg("-END", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noEND");
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", false));
    BOOST_CHECK(!GetBoolArg("-END", true));

    ResetArgs("-noEND=1");
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", false));
    BOOST_CHECK(!GetBoolArg("-END", true));

    ResetArgs("-END -noEND");  // -END should win
    BOOST_CHECK(GetBoolArg("-END"));
    BOOST_CHECK(GetBoolArg("-END", false));
    BOOST_CHECK(GetBoolArg("-END", true));

    ResetArgs("-END=1 -noEND=1");  // -END should win
    BOOST_CHECK(GetBoolArg("-END"));
    BOOST_CHECK(GetBoolArg("-END", false));
    BOOST_CHECK(GetBoolArg("-END", true));

    ResetArgs("-END=0 -noEND=0");  // -END should win
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", false));
    BOOST_CHECK(!GetBoolArg("-END", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--END=1");
    BOOST_CHECK(GetBoolArg("-END"));
    BOOST_CHECK(GetBoolArg("-END", false));
    BOOST_CHECK(GetBoolArg("-END", true));

    ResetArgs("--noEND=1");
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", false));
    BOOST_CHECK(!GetBoolArg("-END", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-END", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-END", "eleven"), "eleven");

    ResetArgs("-END -bar");
    BOOST_CHECK_EQUAL(GetArg("-END", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-END", "eleven"), "");

    ResetArgs("-END=");
    BOOST_CHECK_EQUAL(GetArg("-END", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-END", "eleven"), "");

    ResetArgs("-END=11");
    BOOST_CHECK_EQUAL(GetArg("-END", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-END", "eleven"), "11");

    ResetArgs("-END=eleven");
    BOOST_CHECK_EQUAL(GetArg("-END", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-END", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-END", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-END", 0), 0);

    ResetArgs("-END -bar");
    BOOST_CHECK_EQUAL(GetArg("-END", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-END=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-END", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-END=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-END", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--END");
    BOOST_CHECK_EQUAL(GetBoolArg("-END"), true);

    ResetArgs("--END=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-END", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noEND");
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", true));
    BOOST_CHECK(!GetBoolArg("-END", false));

    ResetArgs("-noEND=1");
    BOOST_CHECK(!GetBoolArg("-END"));
    BOOST_CHECK(!GetBoolArg("-END", true));
    BOOST_CHECK(!GetBoolArg("-END", false));

    ResetArgs("-noEND=0");
    BOOST_CHECK(GetBoolArg("-END"));
    BOOST_CHECK(GetBoolArg("-END", true));
    BOOST_CHECK(GetBoolArg("-END", false));

    ResetArgs("-END --noEND");
    BOOST_CHECK(GetBoolArg("-END"));

    ResetArgs("-noEND -END"); // END always wins:
    BOOST_CHECK(GetBoolArg("-END"));
}

BOOST_AUTO_TEST_SUITE_END()
