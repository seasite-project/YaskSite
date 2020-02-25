/*****************************************************************************

YASK: Yet Another Stencil Kernel
Copyright (c) 2014-2017, Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*****************************************************************************/

#ifndef YASK_UTILS
#define YASK_UTILS

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include <stdexcept>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef WIN32
#define _Pragma(x)
#endif

#if defined(__GNUC__) && !defined(__ICC)
#define __assume(x) ((void)0)
#define __declspec(x)
#endif

#if (defined(__GNUC__) && !defined(__ICC)) || defined(WIN32)
#define restrict
#define __assume_aligned(p,n) ((void)0)
#endif

// VTune or stubs.
#ifdef USE_VTUNE
#include "ittnotify.h"
#define VTUNE_PAUSE  __itt_pause()
#define VTUNE_RESUME __itt_resume()
#else
#define VTUNE_PAUSE ((void)0)
#define VTUNE_RESUME ((void)0)
#endif

// MPI or stubs.
#ifdef USE_MPI
#include "mpi.h"
#else
#define MPI_PROC_NULL (-1)
#define MPI_Barrier(comm) ((void)0)
#define MPI_Comm int
#define MPI_Finalize() ((void)0)
#endif

// OpenMP or stubs.
#ifdef _OPENMP
#include <omp.h>
#else
inline int omp_get_num_procs() { return 1; }
inline int omp_get_num_threads() { return 1; }
inline int omp_get_max_threads() { return 1; }
inline int omp_get_thread_num() { return 0; }
inline void omp_set_num_threads(int n) { }
inline void omp_set_nested(int n) { }
#endif

// rounding macros for integer types.
#define CEIL_DIV(numer, denom) (((numer) + (denom) - 1) / (denom))
#define ROUND_UP(n, mult) (CEIL_DIV(n, mult) * (mult))

namespace yask {

#ifdef USE_MPI
    inline void exit_yask(int code) {
        int flag;
        MPI_Initialized(&flag);
        if (flag)
            MPI_Abort(MPI_COMM_WORLD, code);
        else
            exit(code);
    }
#else
    inline void exit_yask(int code) { exit(code); }
#endif

    // Some utility functions.
    extern double getTimeInSecs();
    extern std::string printWithPow2Multiplier(double num);
    extern std::string printWithPow10Multiplier(double num);

    // Find sum of rank_vals over all ranks.
    extern idx_t sumOverRanks(idx_t rank_val, MPI_Comm comm);

    // Make sure rank_val is same over all ranks.
    void assertEqualityOverRanks(idx_t rank_val, MPI_Comm comm,
                                 const std::string& descr);
    
    // Round up val to a multiple of mult.
    // Print a message if rounding is done.
    extern idx_t roundUp(std::ostream& os,
                         idx_t val, idx_t mult, const std::string& name);

    // Fix inner_size, if needed, to fit into outer_size and be a multiple
    // of mult.  Return number of inner_sized things.
    extern idx_t findNumSubsets(std::ostream& os,
                                idx_t& inner_size, const std::string& inner_name,
                                idx_t outer_size, const std::string& outer_name,
                                idx_t mult, const std::string& dim);
    inline idx_t findNumSubBlocksInBlock(std::ostream& os, idx_t& sbsize, idx_t bsize,
                                         idx_t mult, const std::string& dim) {
        return findNumSubsets(os, sbsize, "sub-block", bsize, "block", mult, dim);
    }
    inline idx_t findNumSubBlocksInSubBlockGroup(std::ostream& os, idx_t& sbsize, idx_t sbgsize,
                                                 idx_t mult, const std::string& dim) {
        return findNumSubsets(os, sbsize, "sub-block", sbgsize, "sub-block-group", mult, dim);
    }
    inline idx_t findNumSubBlockGroupsInBlock(std::ostream& os, idx_t& sbgsize, idx_t bsize,
                                              idx_t mult, const std::string& dim) {
        return findNumSubsets(os, sbgsize, "sub-block-group", bsize, "block", mult, dim);
    }
    inline idx_t findNumBlocksInDomain(std::ostream& os, idx_t& bsize, idx_t dsize,
                                       idx_t mult, const std::string& dim) {
        return findNumSubsets(os, bsize, "block", dsize, "rank-domain", mult, dim);
    }
    inline idx_t findNumBlocksInRegion(std::ostream& os, idx_t& bsize, idx_t rsize,
                               idx_t mult, const std::string& dim) {
        return findNumSubsets(os, bsize, "block", rsize, "region", mult, dim);
    }
    inline idx_t findNumBlocksInBlockGroup(std::ostream& os, idx_t& bsize, idx_t gsize,
                                      idx_t mult, const std::string& dim) {
        return findNumSubsets(os, bsize, "block", gsize, "block-group", mult, dim);
    }
    inline idx_t findNumBlockGroupsInRegion(std::ostream& os, idx_t& gsize, idx_t rsize,
                               idx_t mult, const std::string& dim) {
        return findNumSubsets(os, gsize, "block-group", rsize, "region", mult, dim);
    }
    inline idx_t findNumRegionsInDomain(std::ostream& os, idx_t& rsize, idx_t dsize,
                                idx_t mult, const std::string& dim) {
        return findNumSubsets(os, rsize, "region", dsize, "rank-domain", mult, dim);
    }

    // A class to parse command-line args.
    class CommandLineParser {

    public:

        // Base class for an allowed option.
        class OptionBase {
        protected:
            std::string _name;
            std::string _help;
            std::string _help_leader;
            std::string _current_value_str;

            // Internal function to print help.
            virtual void _print_help(std::ostream& os,
                                     const std::string& str,
                                     int width) const;

            // Check for matching option to str at args[argi].
            // Return true and increment argi if match.
            virtual bool _check_arg(std::vector<std::string>& args, int& argi,
                                    const std::string& str) const;

            // Get one idx_t value from args[argi++].
            // Exit on failure.
            virtual idx_t _idx_val(std::vector<std::string>& args, int& argi);
            
        public:
            OptionBase(const std::string& name,
                       const std::string& help_msg) :
                _name(name), _help(help_msg),
                _help_leader("    "),
                _current_value_str("Current value = ")
            { }
            virtual ~OptionBase() { }

            // Accessors.
            virtual const std::string& get_name() const {
                return _name;
            }
            virtual const std::string& get_help() const {
                return _help;
            }

            // Print help on this option.
            virtual void print_help(std::ostream& os,
                                    int width) const {
                _print_help(os, _name, width);
            }

            // Check for matching option and any needed args at args[argi].
            // Return true, set val, and increment argi if match.
            virtual bool check_arg(std::vector<std::string>& args, int& argi) =0;
        };

        // An allowed boolean option.
        class BoolOption : public OptionBase {
            bool& _val;
            
        public:
            BoolOption(const std::string& name,
                       const std::string& help_msg,
                       bool& val) :
                OptionBase(name, help_msg), _val(val) { }

            virtual void print_help(std::ostream& os,
                                    int width) const;
            virtual bool check_arg(std::vector<std::string>& args, int& argi);
        };

        // An allowed int option.
        class IntOption : public OptionBase {
            int& _val;
            
        public:
            IntOption(const std::string& name,
                      const std::string& help_msg,
                      int& val) :
                OptionBase(name, help_msg), _val(val) { }

            virtual void print_help(std::ostream& os,
                                    int width) const;
            virtual bool check_arg(std::vector<std::string>& args, int& argi);
        };

        // An allowed idx_t option.
        class IdxOption : public OptionBase {
            idx_t& _val;
            
        public:
            IdxOption(const std::string& name,
                       const std::string& help_msg,
                       idx_t& val) :
                OptionBase(name, help_msg), _val(val) { }

            virtual void print_help(std::ostream& os,
                                    int width) const;
            virtual bool check_arg(std::vector<std::string>& args, int& argi);
        };

        // An allowed idx_t option that sets multiple vars.
        class MultiIdxOption : public OptionBase {
            std::vector<idx_t*> _vals;
            
        public:
            MultiIdxOption(const std::string& name,
                           const std::string& help_msg,
                           std::vector<idx_t*> vals) :
                OptionBase(name, help_msg), _vals(vals) {
                _current_value_str = "Current values = ";
            }
            MultiIdxOption(const std::string& name,
                      const std::string& help_msg,
                      idx_t& val0, idx_t& val1, idx_t& val2) :
                OptionBase(name, help_msg) {
                _current_value_str = "Current values = ";
                _vals.push_back(&val0);
                _vals.push_back(&val1);
                _vals.push_back(&val2);
            }

            virtual void print_help(std::ostream& os,
                                    int width) const;
            virtual bool check_arg(std::vector<std::string>& args, int& argi);
        };

    protected:
        std::map<std::string, OptionBase*> _opts;
        int _width;

    public:

        // Ctor.
        CommandLineParser() : _width(78) { }

        // Dtor.
        ~CommandLineParser() {

            // Delete options.
            for (auto i : _opts) {
                delete i.second;
            }
        }

        // Set help width.
        virtual void set_width(int width) {
            _width = width;
        }
        
        // Add an allowed option.
        // Options will be deleted upon destruction.
        virtual void add_option(OptionBase* opt) {
            _opts[opt->get_name()] = opt;
        }

        // Print help info on all options.
        virtual void print_help(std::ostream& os) const;
        
        // Parse options from the command-line and set corresponding vars.
        // Recognized strings from args are consumed, and unused ones
        // remain for further processing by the application.
        virtual void parse_args(const std::string& pgmName,
                                std::vector<std::string>& args);

        // Same as above, but pgmName is populated from argv[0]
        // and args is appended from remainder of argv array.
        // Unused strings are returned in args vector.
        virtual void parse_args(int argc, char** argv,
                                std::vector<std::string>& args) {
            std::string pgmName = argv[0];
            for (int i = 1; i < argc; i++)
                args.push_back(argv[i]);
            parse_args(pgmName, args);
        }

        // Tokenize args from a string.
        virtual void set_args(std::string arg_string,
                              std::vector<std::string>& args);
    };
}

#endif
