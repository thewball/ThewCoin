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
    ResetArgs("-THC");
    BOOST_CHECK(GetBoolArg("-THC"));
    BOOST_CHECK(GetBoolArg("-THC", false));
    BOOST_CHECK(GetBoolArg("-THC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BARo"));
    BOOST_CHECK(!GetBoolArg("-BARo", false));
    BOOST_CHECK(GetBoolArg("-BARo", true));

    ResetArgs("-THC=0");
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", false));
    BOOST_CHECK(!GetBoolArg("-THC", true));

    ResetArgs("-THC=1");
    BOOST_CHECK(GetBoolArg("-THC"));
    BOOST_CHECK(GetBoolArg("-THC", false));
    BOOST_CHECK(GetBoolArg("-THC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", false));
    BOOST_CHECK(!GetBoolArg("-THC", true));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", false));
    BOOST_CHECK(!GetBoolArg("-THC", true));

    ResetArgs("-THC -noBAR");  // -THC should win
    BOOST_CHECK(GetBoolArg("-THC"));
    BOOST_CHECK(GetBoolArg("-THC", false));
    BOOST_CHECK(GetBoolArg("-THC", true));

    ResetArgs("-THC=1 -noBAR=1");  // -THC should win
    BOOST_CHECK(GetBoolArg("-THC"));
    BOOST_CHECK(GetBoolArg("-THC", false));
    BOOST_CHECK(GetBoolArg("-THC", true));

    ResetArgs("-THC=0 -noBAR=0");  // -THC should win
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", false));
    BOOST_CHECK(!GetBoolArg("-THC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--THC=1");
    BOOST_CHECK(GetBoolArg("-THC"));
    BOOST_CHECK(GetBoolArg("-THC", false));
    BOOST_CHECK(GetBoolArg("-THC", true));

    ResetArgs("--noBAR=1");
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", false));
    BOOST_CHECK(!GetBoolArg("-THC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-THC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-THC", "eleven"), "eleven");

    ResetArgs("-THC -bar");
    BOOST_CHECK_EQUAL(GetArg("-THC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-THC", "eleven"), "");

    ResetArgs("-THC=");
    BOOST_CHECK_EQUAL(GetArg("-THC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-THC", "eleven"), "");

    ResetArgs("-THC=11");
    BOOST_CHECK_EQUAL(GetArg("-THC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-THC", "eleven"), "11");

    ResetArgs("-THC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-THC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-THC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-THC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-THC", 0), 0);

    ResetArgs("-THC -bar");
    BOOST_CHECK_EQUAL(GetArg("-THC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-THC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-THC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-THC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-THC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--THC");
    BOOST_CHECK_EQUAL(GetBoolArg("-THC"), true);

    ResetArgs("--THC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-THC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", true));
    BOOST_CHECK(!GetBoolArg("-THC", false));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-THC"));
    BOOST_CHECK(!GetBoolArg("-THC", true));
    BOOST_CHECK(!GetBoolArg("-THC", false));

    ResetArgs("-noBAR=0");
    BOOST_CHECK(GetBoolArg("-THC"));
    BOOST_CHECK(GetBoolArg("-THC", true));
    BOOST_CHECK(GetBoolArg("-THC", false));

    ResetArgs("-THC --noBAR");
    BOOST_CHECK(GetBoolArg("-THC"));

    ResetArgs("-noBAR -THC"); // THC always wins:
    BOOST_CHECK(GetBoolArg("-THC"));
}

BOOST_AUTO_TEST_SUITE_END()
