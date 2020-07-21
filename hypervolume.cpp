#include "hypervolume.h"
#include "array_objective_solution.h"
#include "core.h"
#include <iostream>
#include <limits>


Hypervolume::Hypervolume()
{
}


Hypervolume::~Hypervolume()
{

}

char* Hypervolume::execCommand(const char* command){

    FILE* fp;
    char* line = nullptr;
    // Following initialization is equivalent to char* result = ""; and just
    // initializes result to an empty string, only it works with
    // -Werror=write-strings and is so much less clear.
    char* result = (char*) calloc(1, 1);
    size_t len = 0;

    fflush(nullptr);
    fp = popen(command, "r");
    if (fp == nullptr)
    {
        printf("Cannot execute command:\n%s\n", command);
        return nullptr;
    }

    while (getline(&line, &len, fp) != -1)
    {
        // +1 below to allow room for null terminator.
        result = (char*) realloc(result, strlen(result) + strlen(line) + 1);
        // +1 below so we copy the final null terminator.
        strncpy(result + strlen(result), line, strlen(line) + 1);
        free(line);
        line = nullptr;
    }

    fflush(fp);
    if (pclose(fp) != 0)
    {
        perror("Cannot close stream.\n");
    }

    return result;
}

//Delta Metric and hypervolume are working requires Minimization problems
double Hypervolume::hypervolumeWithExecRequested(vector<vector<double> > pareto, vector<double> refPoints, bool minimization){
    int nSol = pareto.size();
    int nObj = pareto[0].size();

    if (minimization == false)
    {
        for (int i = 0; i < nSol; i++)
            for (int ev = 0; ev < nObj; ev++)
                pareto[i][ev] *= -1;
    }

    string tempFile = "tempFileHipervolumeFunc";
    FILE* fTempHV = fopen(tempFile.c_str(), "w");

    for (int s = 0; s < nSol; s++)
    {
        for (int o = 0; o < nObj; o++)
        {
            fprintf(fTempHV, "%.7f\t", pareto[s][o]);
        }
        fprintf(fTempHV, "\n");
    }

    fclose(fTempHV);
    std::stringstream ss;
    ss << "./hv\t -r \"";
    for (int o = 0; o < nObj; o++)
        ss << refPoints[o] << " ";
    ss << "\" \t" << tempFile.c_str();
    string hvValueString = execCommand(ss.str().c_str());
    double hvValue = atof(hvValueString.c_str());
    return hvValue;
}

//Delta Metric and hypervolume are working requires Minimization problems
double Hypervolume::hypervolumeWithExecRequestedbyFile(string pareto_file_name, vector<double> refPoints){
    // START: Mock
    vector<double> min_points = {-100, 0, 0};
    // END: Mock


    ifstream pareto_file(pareto_file_name);

    if ( !pareto_file ){
      cout << "File not found" << endl;
      return -1;
    }

    int nSol, nObj;
    double z;

    pareto_file >> nSol;
    pareto_file >> nObj;    

    string tempFile = "tempFileHipervolumeFunc";
    FILE* fTempHV = fopen(tempFile.c_str(), "w");

    for (int s = 0; s < nSol; s++)
    {
        for (int o = 0; o < nObj; o++)
        {
            pareto_file >> z;
            z = (-z - min_points[o])/(-refPoints[o] - min_points[o]);
            fprintf(fTempHV, "%.7f\t", z);
        }
        fprintf(fTempHV, "\n");
    }

    pareto_file.close();
    fclose(fTempHV);
    stringstream ss;
    ss << "./hv\t -r \"";
    for (int o = 0; o < nObj; o++)
        ss << 1 << " ";
//        ss << -refPoints[o] << " ";
    ss << "\" \t" << tempFile.c_str();
    string hvValueString = execCommand(ss.str().c_str());
    double hvValue = atof(hvValueString.c_str());
    return hvValue;
}

int Hypervolume::cardinalite(Array_Objective_Solution * D, Array_Objective_Solution * ref)
{
    int card = 0;
    for (int i = 0; i < D->size(); i++)
    {
        for (int j = 0; j < ref->size(); j++)
        {
            if (D->get(i)->weakly_dominates(ref->get(j)))
            {
                card++;
                j = ref->size();
            }
        }
    }

    return card;
}

double Hypervolume::setCoverage(Array_Objective_Solution * a, Array_Objective_Solution * b)
{
    double cover = 0;

    for (int i = 0; i < b->size(); i++)
    {
        for (int j = 0; j < a->size(); j++)
        {
            bool wD = a->get(j)->weakly_dominates(b->get(i));
            if (wD)
            {
                j = a->size();
                cover += 1;
                //cout<<" i = "<<i<<" j = "<<j<<endl;
                //getchar();
            }
        }
    }

    //cout<<" cover = "<<cover<<endl;

    double sCover = cover / b->size();

    return sCover;
}
