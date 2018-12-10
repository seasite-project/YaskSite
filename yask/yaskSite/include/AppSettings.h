#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include "stencil_calc.hpp"

//THIS file might not be needed

// Add some command-line options for this application to the
// default ones provided by YASK.
struct AppSettings : public StencilSettings {
    bool help;                  // help requested.
    bool doWarmup;              // whether to do warmup run.
    int num_trials;             // number of trials.
    bool validate;              // whether to do validation run.
    int pre_trial_sleep_time;   // sec to sleep before each trial.

    AppSettings() :
        help(false),
        doWarmup(true),
        num_trials(3),
        validate(false),
        pre_trial_sleep_time(1)
    { }

    // A custom option-handler for validation.
    class ValOption : public CommandLineParser::OptionBase {
        AppSettings& _as;

    public:
        ValOption(AppSettings& as) :
                OptionBase("v",
                           "Shortcut for '-validate -no-warmup -t 1 -dt 1 -d 64"
#if USING_DIM_W
                           " -dw 3"
#endif
                           " -b 24'."),
                _as(as) { }

        // Set multiple vars.
        virtual bool check_arg(std::vector<std::string>& args,
                               int& argi) {
            if (_check_arg(args, argi, _name)) {
                _as.validate = true;
                _as.doWarmup = false;
                _as.num_trials = 1;
                _as.dt = 1;
#if USING_DIM_W
                _as.dw = 3;
#endif
                _as.dx = _as.dy = _as.dz = 64;
                _as.bx = _as.by = _as.bz = 24;
                return true;
            }
            return false;
        }
    };

#ifndef DEF_ARGS
#define DEF_ARGS ""
#endif

    // Parse options from the command-line and set corresponding vars.
    // Exit with message on error or request for help.
    void parse(int argc, char** argv) {

        // Create a parser and add base options to it.
        CommandLineParser parser;
        add_options(parser);

        // Add more options for this app.
        parser.add_option(new CommandLineParser::BoolOption("h",
                                         "Print help message.",
                                         help));
        parser.add_option(new CommandLineParser::BoolOption("help",
                                         "Print help message.",
                                         help));
        parser.add_option(new CommandLineParser::BoolOption("warmup",
                                         "Run warmup iteration(s) before performance trial(s).",
                                         doWarmup));
        parser.add_option(new CommandLineParser::IntOption("t",
                                        "Number of performance trials.",
                                        num_trials));
        parser.add_option(new CommandLineParser::IntOption("sleep",
                                        "Number of seconds to sleep before each performance trial.",
                                        pre_trial_sleep_time));
        parser.add_option(new CommandLineParser::BoolOption("validate",
                                         "Run validation iteration(s) after performance trial(s).",
                                         validate));
        parser.add_option(new ValOption(*this));

        // Parse cmd-line options.
        // Any remaining strings will be left in args.
        vector<string> args;
        parser.set_args(DEF_ARGS, args);
        parser.parse_args(argc, argv, args);

        if (help) {
            string appNotes =
                "Validation is very slow and uses 2x memory,\n"
                " so run with very small sizes and number of time-steps.\n"
                " If validation fails, it may be due to rounding error;\n"
                "  try building with 8-byte reals.\n";
            vector<string> appExamples;
            appExamples.push_back("-t 2");
            appExamples.push_back("-v");
            print_usage(cout, parser, argv[0], appNotes, appExamples);
            exit_yask(1);
        }

        if (args.size()) {
            cerr << "Error: extraneous parameter(s):";
            for (auto arg : args)
                cerr << " '" << arg << "'";
            cerr << ".\nRun with '-help' option for usage.\n" << flush;
            exit_yask(1);
        }
    }

    // Print splash banner and invocation string.
    // Exit with help message if requested.
    void splash(ostream& os, int argc, char** argv)
    {
        os <<
            "┌──────────────────────────────────────────┐\n"
            "│  Y.A.S.K. ── Yet Another Stencil Kernel  │\n"
            "│            https://01.org/yask           │\n"
            "│    Intel Corporation, copyright 2017     │\n"
            "└──────────────────────────────────────────┘\n"
            "\nStencil name: " YASK_STENCIL_NAME << endl;

        // Echo invocation parameters for record-keeping.
        os << "Default arguments: " DEF_ARGS << endl;
        os << "Invocation:";
        for (int argi = 0; argi < argc; argi++)
            os << " " << argv[argi];
        os << endl;
    }
};

#endif
