#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <boost/tokenizer.hpp>
#include <algorithm>

using namespace std;

int topK;

void sortContainer( map<int, vector< pair<int, float> > > &container )
{
    for (auto &query : container)
    {
        sort( query.second.begin(), query.second.end(), []( pair<int, float> &left, pair<int, float> &right )
            {
                return left.second < right.second;  
            });
    }
}

void removeItself( map<int, vector< pair<int, float> > > &container )
{
    for ( auto &query : container ) 
    {
        int queryId = query.first;
        for ( int i = 0; i < query.second.size(); ++i)
        {
            if ( queryId == query.second[i].first )
            {
                query.second.erase( query.second.begin() + i );
                break; 
            }
        }
    }
}

void removeExtra( map<int, vector< pair<int, float> > > &container )
{
    for (auto &query : container)
    {
        if ( query.second.size() > topK )
        {
            query.second.erase( query.second.begin() + topK, query.second.end() );
        }
    }
}

int main(int argc, char** argv)
{
    int numOfThreads = atoi(argv[1]);
    topK = 20;
    
    map<int, vector< pair<int, float> > > groundTruth;
    map<int, vector< pair<int, float> > > appResult;

    // Read in groundtruth file
    string line;
    string fileName = argv[3];
    ifstream lshboxFile(fileName);
    if ( lshboxFile.is_open() )
    {
        getline(lshboxFile, line); // The first line indicating that we have 1000 queries, each has top-K nearest items.
        while ( getline(lshboxFile, line) )
        {
            int queryId;
            int itemId;
            float distance;
            vector< pair<int, float> > oneResult;
           
            boost::char_separator<char> sep("\t");
            boost::tokenizer< boost::char_separator<char> > tok(line, sep);
            boost::tokenizer< boost::char_separator<char> >::iterator it = tok.begin(); 
            queryId = stoi(*it++);
            while ( it != tok.end() )
            {
                itemId = stoi(*it++);
                if ( it == tok.end() )
                {
                    break;
                }
                distance = stof(*it++);
                if ( queryId == itemId )
                {
                    continue;
                }
                oneResult.push_back( make_pair(itemId, distance) );
            }
            groundTruth[queryId] = oneResult;    
        }
        lshboxFile.close();
    }
    else
    {
        cout << "Unable to open lshbox file!\n";
    }

    // DEBUG ONLY
    //cout << "groundtruth.size(): " << groundTruth.size() << "\n";
    //
    //for (const auto &query : groundTruth)
    //{
    //    cout << query.first << "\n";
    //    for (const auto &item : query.second)
    //    {
    //        cout << item.first << "\t";
    //        cout << item.second << "\n";
    //    }
    //    cout << "\n";
    //    return 0;
    //}
    //return 0;
    
    // Read in app result
    for ( int partIter = 0; partIter < numOfThreads; ++partIter)
    {
        line = "";
        fileName = argv[2] + to_string(partIter);
        ifstream resultFile(fileName);
        if ( resultFile.is_open() )
        {
            while ( getline(resultFile, line) )
            {            
                int queryId;
                int itemId;
                float distance;
                vector< pair<int, float> > oneResult;

                boost::char_separator<char> sep(" ");
                boost::tokenizer< boost::char_separator<char> > tok(line, sep);
                boost::tokenizer< boost::char_separator<char> >::iterator it = tok.begin(); 
                queryId = stoi(*it++);
                while ( it != tok.end() )
                {
                    itemId = stoi(*it++);
                    if ( it == tok.end() )
                    {
                        break;
                    }
                    distance = stof(*it++);
                    if ( queryId == itemId )
                    {
                        continue;
                    }
                    oneResult.push_back( make_pair(itemId, distance) );
                }
                appResult[queryId] = oneResult;
            }
            resultFile.close();
        }
        else
        {
            cout << "Unable to open Input Result!\n";
        }
    }

    // DEBUG ONLY
    //cout << "appResult.size(): " << appResult.size() << "\n";

    // Sort appResult, groundTruth
    sortContainer(appResult);
    sortContainer(groundTruth);

    // Remove query itself from result
    removeItself(appResult);
    removeItself(groundTruth);
    
    // Remove extra item in appResult
    removeExtra(appResult);
    removeExtra(groundTruth);

    // Each query should have $topK elements in vector<int, float> on both appResult and goundtruth, not containing itself.

    // Calculate Error Ratio
    int numberOfQuery = 0;
    float recallSum = 0;
    string recallString = "";
    for ( const auto &query : appResult )
    {
        
        numberOfQuery++;

        // Check app result size() is right
        if ( query.second.size() > topK )
        {
            cout << "Wow! item.second.size() is more than " + to_string(topK) + "!\n";
            return 0;
        }
        int queryId = query.first;
        
        // DEBUG ONLY: Check queryId exist in groundTruth
        map<int, vector< pair<int, float> > >::iterator queryExist;
        queryExist = groundTruth.find(queryId);
        if (queryExist == groundTruth.end() )
        {
            cout << "Error! Some queryId is not found in groundTruth!\n";
            return 0; 
        }
        //// DEBUG ONLY:  
        if ( query.second.size() > groundTruth.at(queryId).size() )
        {
            cout << "Error! Index will be out of bound!\n";  
            return 0; 
        }
       
        vector< pair<int, float> > oldRowGT = groundTruth.at(queryId);
        vector< pair<int, float> > oldRowAR = query.second;

        vector<int> newRowGT;
        vector<int> newRowAR;
        
        for ( int i = 0; i < oldRowGT.size(); ++i ) 
        {
            newRowGT.push_back( oldRowGT[i].first );
        }
        for ( int i = 0; i < oldRowAR.size(); ++i )
        {
            newRowAR.push_back( oldRowAR[i].first );
        }

        // DEBUG ONLY
        if ( newRowAR.size() > topK ){
            cout << "Error! newRowAR.size: " << newRowAR.size() << "\n";
            return 0;
        }
        if ( newRowGT.size() > topK ){
            cout << "Error! newRowGT.size: " << newRowGT.size() << "\n";
            return 0;
        }
        // DEBUG ONLY: check redundancy in AR
        for ( int x = 0; x < (newRowAR.size() - 1); x++ )
        {
            for ( int y = x + 1; y < newRowAR.size(); y++ )
            {
                if ( newRowAR[x] == newRowAR[y] )
                {
                    cout << "Redundancy found! One neighbor was found twice for one query. queryId: " << queryId << "\n";
                    return 0;
                }
            } 
        }

        float oneRecall = 0;
        int trueNeighbors = 0;
        for ( auto itemId : newRowAR )
        {
            if ( find( newRowGT.begin(), newRowGT.end(), itemId ) != newRowGT.end() )
            {
                trueNeighbors++;
            } 
        }
        oneRecall = (float)trueNeighbors / (float)topK;
        recallString += to_string(oneRecall) + " ";
        recallSum += oneRecall;
    }

    float finalRecall = recallSum / numberOfQuery;

    cout << "recallSum: " << recallSum << "\n";
    cout << "numberOfQuery: " << numberOfQuery << "\n";
    cout << "Final recall: " << finalRecall << "\n";
    //cout << recallString << "\n";

    //string outputName = argv[4];
    //ofstream outputFile;
    //outputFile.open( outputName );
    //if ( outputFile.is_open() )
    //{
    //    outputFile << "Final Recall: " << finalRecall << "\n";
    //    outputFile << "Query Number: " << numberOfQuery << "\n";
    //    outputFile << "recallSum: " << recallSum << "\n";    
    //}
    //else
    //{
    //    cout << "Error. Cannot open output file. \n";
    //    return 0;
    //}
    //outputFile.close();

    return 0;
}
