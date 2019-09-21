#ifndef YASKSITE_PERF_MODEL_H
#define YASKSITE_PERF_MODEL_H

#include "types.h"
#include <vector>
#include <map>
#include <string>

struct blockDetails
{
    cache_info* fit_domain;
    cache_info* temporal;
    cache_info* spatialOBC;
    cache_info* spatialIBC;
};

std::vector<std::string> readIACAout(char* iacaFile, char *entity);

void printECM_style(std::vector<double> ECM, std::vector<double> ECM_data, double ECM_penalty, std::vector<double> ECM_latency={}, std::vector<double> ECM_prefetch={},std::vector<double> ECM_assoc={}, std::vector<double> ECM_boundary={}, std::vector<double> ECM_prefetch_cy={}, std::vector<double> ECM_boundary_cy={}, std::vector<double> ECM_assoc_cy={});

void setCounter(std::string groupName, std::string metricName, int *curr_gid, int *curr_metric_id);

double utilization(double nthreads, double tcomp, double tdata, double tL3Mem);

class perfModel
{
    STENCIL *stencil;
    int numReadGrids;
    int numWriteGrids;
    int numStencils;
   // double cpu_freq;
    //port cycles
    std::map<std::string, double> portCycle;
    //data cycles
    std::map<std::string, std::vector<double>> bytePerCycle;
    int blockThroughput;
    std::vector<std::string> LD_ports;
    void parseIACA(char *iacaFile);
    void calc_ECM(int scale, int temporalStoreMode=0);
    std::vector<double> addBlockBoundaryEffects(cache_info cache, bool temporal=false);
    double getPrefetchEffects(cache_info cache, blockDetails* opt);
    double getLatencyEffects(cache_info cache, int rwRatio, bool temporal=false, int temporalNthreads=1);
    //void getBytePerCycles();
    std::vector<double> getDataContrib(char* cache_str, blockDetails* opt, int temporalStoreMode);
    std::vector<double> simulateCache_assoc(bool &ilc_jump, bool &olc_jump, cache_info cache, double shrink_size=1);

    std::vector<double> spatialOh;
    std::vector<double> temporalOh;
    bool LC_violated(cache_info cache_str, LC type);
    blockDetails determineBlockDetails();
    double applyNOL();
    void validate();
    bool likwidInited;
    int *cpus;
    //id of measured group
    std::vector<std::string> gName;
    std::vector<std::string> metricName;
    std::vector<int> gid;
    std::vector<int> metric_id;

    std::vector<int> eventOffset;

    std::vector<double> readFreq(int eOffsetId);
    double readDataVol(int eOffsetId);

    double chosen_mem_l3_bw;
    double weight; //Used for concatenating eqnGroups, this weight is used while joining them
    bool derived; //is it a pure kernel or a derived(combination)
    bool val;

    public:
    double cpu_freq;
    std::vector<double> ECM;
    /*Latency cycles*/
    std::vector<double> ECM_latency;
    /*Prefetch bytes*/
    std::vector<double> ECM_prefetch;
    /*Prefetch cy*/
    std::vector<double> ECM_prefetch_cy;
    /*Boundary bytes*/
    std::vector<double> ECM_boundary;
    /*Boundary cy*/
    std::vector<double> ECM_boundary_cy;
    /*Assoc bytes*/
    std::vector<double> ECM_assoc;
    /*Assoc cy*/
    std::vector<double> ECM_assoc_cy;

    double ECM_penalty; //used in temporal blocking 2 account for 2 phased execution
    double ECM_extra_work_percent;
    std::vector<double> ECM_validate;
    std::vector<double> ECM_data;
    std::vector<double> ECM_validate_data;

    contribution ecm_contribution;
    perfModel(STENCIL *stencil_, double cpu_freq, char* iacaOut_=NULL);
    ~perfModel();

    void setReadWriteGrids(int numReadGrids_, int numWriteGrids_, int numStencils_);
    void setWeight(double weight_);
    void setDerived(bool derived_);

    void model(int scale=1, bool validate=false);
    void printECM(const char* title="");
    double getPerf();
    std::vector<double> getSaturation();

    double calcTemporalExtraWork();

    //To add(stich) models together
    friend perfModel operator+(const perfModel& a, const perfModel& b);
};

#endif
