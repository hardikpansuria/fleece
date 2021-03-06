//
//  CaseListReporter.hh
//  Couchbase Lite Core
//
//  Created by Jens Alfke on 8/26/16.
//  Copyright (c) 2016 Couchbase. All rights reserved.
//

#include "catch.hpp"
#include "Backtrace.hh"
#include <time.h>


/** Custom reporter that logs a line for every test file and every test case being run.
    Use CLI args "-r list" to use it. */
struct CaseListReporter : public Catch::ConsoleReporter {
    CaseListReporter( Catch::ReporterConfig const& _config )
    :   Catch::ConsoleReporter( _config )
    {
        auto now = time(nullptr);
        stream << "STARTING TESTS AT " << ctime(&now) << "\n";
    }

    virtual ~CaseListReporter() CATCH_OVERRIDE {
        auto now = time(nullptr);
        stream << "ENDED TESTS AT " << ctime(&now) << "\n";
    }

    static std::string getDescription() {
        return "Logs a line for every test case";
    }

    virtual void testCaseStarting( Catch::TestCaseInfo const& _testInfo ) CATCH_OVERRIDE {
        auto file = _testInfo.lineInfo.file;
        if (file != _curFile) {
            _curFile = file;
            auto slash = file.rfind('/');
            stream << "## " << file.substr(slash+1) << ":\n";
        }
        stream << "\t>>> " << _testInfo.name << "\n";
        _firstSection = true;
        _sectionNesting = 0;
        ConsoleReporter::testCaseStarting(_testInfo);
    }
    virtual void testCaseEnded( Catch::TestCaseStats const& _testCaseStats ) CATCH_OVERRIDE {
        ConsoleReporter::testCaseEnded(_testCaseStats);
    }

    virtual void sectionStarting( Catch::SectionInfo const& _sectionInfo ) CATCH_OVERRIDE {
        if (_firstSection)
            _firstSection = false;
        else {
            for (unsigned i = 0; i < _sectionNesting; ++i)
                stream << "\t";
            stream << "\t--- " << _sectionInfo.name << "\n";
        }
        ++_sectionNesting;
        ConsoleReporter::sectionStarting(_sectionInfo);
    }

    void sectionEnded( Catch::SectionStats const& sectionStats ) CATCH_OVERRIDE {
        --_sectionNesting;
        ConsoleReporter::sectionEnded(sectionStats);
    }

    virtual bool assertionEnded( Catch::AssertionStats const& stats ) CATCH_OVERRIDE {
        if (stats.assertionResult.getResultType() == Catch::ResultWas::FatalErrorCondition) {
            std::cerr << "\n\n********** CRASH: "
                      << stats.assertionResult.getMessage()
                      << " **********";
            fleece::Backtrace bt(5);
            bt.writeTo(std::cerr);
            std::cerr << "\n********** CRASH **********\n";
        }
        return Catch::ConsoleReporter::assertionEnded(stats);
    }

    std::string _curFile;
    bool _firstSection;
    unsigned _sectionNesting;
};

REGISTER_REPORTER( "list", CaseListReporter )
