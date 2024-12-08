#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <chrono>
#include <algorithm>
// #include <iostream>
// #include <fstream>
// #include <vector>
#include <ctime>
// #include <cstdlib>


// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

public:
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
	
		nvalues=n;
		values=vals;
		

	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }



};


 // The whole network represted as a list of nodes
class network{
	friend float calc_change(network& before, network& after); // Declare calc_change as a friend

	list <Graph_Node> Pres_Graph;

public:
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
// get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }



	float calc_change(network& before, network& after) {
		float maxChange = 0.0;

		list<Graph_Node>::iterator beforeNode, afterNode;

		for (beforeNode = before.Pres_Graph.begin(), afterNode = after.Pres_Graph.begin();
			beforeNode != before.Pres_Graph.end() && afterNode != after.Pres_Graph.end();
			++beforeNode, ++afterNode) {
			vector<float> beforeCPT = beforeNode->get_CPT();
			vector<float> afterCPT = afterNode->get_CPT();

			// Calculate the maximum change in CPT values for this node
			for (int i = 0; i < beforeCPT.size(); i++) {
				float change = abs(beforeCPT[i] - afterCPT[i]);
				if (change > maxChange) {
					maxChange = change;
				}
			}
		}

		return maxChange;
	}


};


class Dataset {
private:
    // Data matrix: Each row represents a patient record
    // If an entry is -1, it denotes a missing value ('?')
    vector<vector<int>> data_matrix;

    // Map a value (e.g., "True") to an integer (e.g., 0) for fast retrieval
    unordered_map<string, int> value_map;

    // Reverse map for converting back from integers to values
    vector<string> value_list;

    // Store the lines and positions of question marks.
    // E.g. If question mark is in line 3 at position 2, we store it as {3, 2}
    vector<pair<int, int>> missing_values_positions;

public:
    // Constructor
    Dataset() {}
	
	int get_size(){
		return data_matrix.size() ; 
	}
    // Add a new patient record
    void add_record(const vector<string>& record) {
        vector<int> int_record;
        for (int i = 0; i < record.size(); ++i) {
            const string& value = record[i];
            if (value == "?") {
                int_record.push_back(-1);
                missing_values_positions.push_back({static_cast<int>(data_matrix.size()), i});
            } else {
                if (value_map.find(value) == value_map.end()) {
                    value_map[value] = value_list.size();
                    value_list.push_back(value);
                }
                int_record.push_back(value_map[value]);
            }
        }
        data_matrix.push_back(int_record);
    }

    // Retrieve a specific patient record
    vector<string> get_record(int index) const {
        const vector<int>& int_record = data_matrix[index];
        vector<string> record;
        for (int value : int_record) {
            if (value == -1) {
                record.push_back("?");
            } else {
                record.push_back(value_list[value]);
            }
        }
        return record;
    }

    // Retrieve the positions of all missing values
    const vector<pair<int, int>>& get_missing_values_positions() const {
        return missing_values_positions;
    }



    // Other utility functions can be added as needed...
	// check the below fucntion ?
	void update_value(int row, int col, const string& value) {
        if (row < data_matrix.size() && col < data_matrix[row].size()) {
            if (value == "?") {
                data_matrix[row][col] = -1;
            } else {
                if (value_map.find(value) == value_map.end()) {
                    value_map[value] = value_list.size();
                    value_list.push_back(value);
                }
                data_matrix[row][col] = value_map[value];
            }
        }
    }
};


vector<string> split_record(const string& line) {
    // Remove the first and last quote
    string trimmed = line.substr(1, line.size() - 2);

    // Split the string by the delimiter '" "'
    vector<string> record;
    size_t start = 0;
    size_t end = trimmed.find("\" \"");
    
    while (end != string::npos) {
        record.push_back(trimmed.substr(start, end - start));
        start = end + 3; // move past the found delimiter
        end = trimmed.find("\" \"", start);
    }
    
    // Push the last value after the last delimiter
    record.push_back(trimmed.substr(start));
    
    return record;
}

void read_data_file(const string& filename, Dataset& dataset) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        vector<string> record = split_record(line);
        dataset.add_record(record);
    }

    file.close();
}



network read_network()
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile("alarm.bif"); 
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<float> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                        
     					curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;
                       
                        

    				}
                    
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                
            }
     		
     		

    		
    		
    	}
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}



// void random_initialise_data(const string &inputDataset, const string &outputDataset, network &Alarm) {
//     ifstream input(inputDataset);
//     ofstream output(outputDataset);

//     if (!input.is_open() || !output.is_open()) {
//         cout << "Failed to open input or output dataset file." << endl;
//         return;
//     }

//     string line;
//     while (getline(input, line)) {
//         vector<string> record;
//         stringstream ss(line);
//         string value;
//         int missingIndex = -1; // Index of the missing value

//         int index = 0;
//         while (ss >> value) {
//             if (value == "\"?\"") {
//                 missingIndex = index;
//                 record.push_back(value); // Placeholder for the missing value
//             } else {
//                 record.push_back(value);
//             }
//             index++;
//         }

//         if (missingIndex != -1) {
//             // Find the corresponding node in the Alarm network
//             list<Graph_Node>::iterator node = Alarm.get_nth_node(missingIndex);
//             vector<string> possibleValues = node->get_values();
            
//             // Randomly select a value from the possible values
//             int randomIndex = rand() % possibleValues.size();
//             record[missingIndex] = possibleValues[randomIndex];
//         }

//         // Write the updated record to the output dataset
//         output << record[0];
//         for (int i = 1; i < record.size(); i++) {
//             output << " " << record[i];
//         }
//         output << endl;
//     }

//     input.close();
//     output.close();
// }

void evaluate_CPT(network& Alarm, Dataset& dataset2) {
    int numSamples = dataset2.get_size(); // Get the number of samples in dataset2

    for (int nodeIndex = 0; nodeIndex < Alarm.netSize(); nodeIndex++) {
        list<Graph_Node>::iterator currentNode = Alarm.get_nth_node(nodeIndex);
        int nvalues = currentNode->get_nvalues();
        vector<float> CPT(nvalues, 0.0);
        vector<float> counts(nvalues, 1.0);

        // Iterate through the dataset2 records to update CPT
        for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++) {
            vector<string> record = dataset2.get_record(sampleIndex);

            // Check if the record matches the parents' values for the current node
            bool matches = true;
            for (int i = 0; i < currentNode->get_Parents().size(); i++) {
                int parentIndex = Alarm.get_index(currentNode->get_Parents()[i]);
                if (record[parentIndex] != currentNode->get_values()[i]) {
                    matches = false;
                    break;
                }
            }

            if (matches) {
                // Increment the count for the corresponding value of the current node
                int valueIndex = distance(currentNode->get_values().begin(),
                    find(currentNode->get_values().begin(), currentNode->get_values().end(), record[nodeIndex]));
                counts[valueIndex]++;
            }
        }

        // Calculate the CPT with Laplace smoothing
        float total = 0;
        for (int i = 0; i < nvalues; i++) {
            CPT[i] = counts[i] / (numSamples + nvalues); // Laplace smoothing
            total += CPT[i];
        }

        // Normalize the CPT values
        for (int i = 0; i < nvalues; i++) {
            CPT[i] /= total;
        }

        currentNode->set_CPT(CPT);
    }
}



void random_initialise_data(Dataset& dataset1, Dataset& dataset2, network& Alarm) {
    const vector<pair<int, int>>& missing_positions = dataset2.get_missing_values_positions();

    for (const auto& position : missing_positions) {
        int row = position.first;
        int col = position.second;

        list<Graph_Node>::iterator node = Alarm.get_nth_node(col);
        vector<string> possibleValues = node->get_values();

        // Randomly select a value from the possible values in dataset1
        int randomIndex = rand() % possibleValues.size();
        string replacementValue = possibleValues[randomIndex];

        // Update the missing value in dataset2
        dataset2.update_value(row, col, replacementValue);
    }
}





// void evaluate_CPT(network &Alarm, const string &dataFile) {
//     ifstream infile(dataFile);
//     if (!infile.is_open()) {
//         cerr << "Error: Unable to open data file." << endl;
//         return;
//     }
//     vector<vector<string>> dataset;
//     string line;
//     while (getline(infile, line)) {
//         stringstream ss(line);
//         string value;
//         vector<string> record;
//         while (ss >> value) {
//             record.push_back(value);
//         }
//         dataset.push_back(record);
//     }
//     infile.close();

//     int numSamples = dataset.size();

//     for (int nodeIndex = 0; nodeIndex < Alarm.netSize(); nodeIndex++) {
//         list<Graph_Node>::iterator currentNode = Alarm.get_nth_node(nodeIndex);
//         int nvalues = currentNode->get_nvalues();
//         vector<float> CPT(nvalues, 0.0);
//         vector<float> counts(nvalues, 1.0);

// 		// the following part seems wrong. please correct it 

//         // for (vector<string> &record : dataset) {
//         //     bool matches = true;
//         //     for (int i = 0; i < currentNode->get_Parents().size(); i++) {
//         //         int parentIndex = Alarm.get_index(currentNode->get_Parents()[i]);
//         //         if (record[parentIndex] != currentNode->get_values()[i]) {
//         //             matches = false;
//         //             break;
//         //         }
//         //     }

//         //     if (matches) {
//         //         // Increment the count for the corresponding value of the current node
//         //         int valueIndex = distance(currentNode->get_values().begin(),
//         //             find(currentNode->get_values().begin(), currentNode->get_values().end(), record[nodeIndex]));
//         //         counts[valueIndex]++;
//         //     }
//         // }

//         // Calculate the CPT with Laplace smoothing
//         float total = 0;
//         for (int i = 0; i < nvalues; i++) {
//             CPT[i] = counts[i] / (numSamples + nvalues); // Laplace smoothing
//             total += CPT[i];
//         }

//         // Normalize the CPT values
//         for (int i = 0; i < nvalues; i++) {
//             CPT[i] /= total;
//         }

//         currentNode->set_CPT(CPT);
//     }
// }


// Define a function to sample a value from a given probability distribution
string sample_value(const vector<float>& probabilities) {
    if (probabilities.size() == 0) {
        return ""; // Empty probabilities, return an empty string
    }

    // Create a random engine with a seed based on the current time
    unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
    default_random_engine generator(seed);

    // Create a discrete distribution using the probabilities
    discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());

    // Sample a random value
    int sampledIndex = distribution(generator);
    return to_string(sampledIndex); // Convert the index to a string
}

// EM Step function
// void EM_step(network& Alarm, const string& dataFile, const string& newDataFile) {
//     // Read the dataset from the original file
//     ifstream datasetFile(dataFile);
//     if (!datasetFile) {
//         cerr << "Error: Cannot open the dataset file." << endl;
//         return;
//     }

//     // Create a new dataset file for updated records
//     ofstream newDatasetFile(newDataFile);
//     if (!newDatasetFile) {
//         cerr << "Error: Cannot open the new dataset file." << endl;
//         datasetFile.close();
//         return;
//     }

//     vector<string> datasetRecord;
//     string line;
// 	int counter = 0; 

//     // Iterate through the dataset records
//     while (getline(datasetFile, line)) {
//         datasetRecord.clear();
//         istringstream recordStream(line);
//         string value;

//         // Split the record into individual values
//         while (recordStream >> value) {
//             datasetRecord.push_back(value);
//         }

//         // Initialize a variable to track the index of the "?" value
//         int missingIndex = -1;

//         // Find the index of the "?" value, if it exists in the record
//         for (int i = 0; i < datasetRecord.size(); i++) {
//             if (datasetRecord[i] == "?") {
//                 missingIndex = i;
// 				counter++ ; 
//                 break;
//             }
//         }

//         // If there is a "?" in the record, update it
//         if (missingIndex != -1) {
// 			// Initialize a vector to store updated records
// 			vector<string> updatedRecord = datasetRecord;

// 			// Retrieve the node corresponding to the "?" value
// 			list<Graph_Node>::iterator node = Alarm.get_nth_node(missingIndex);
// 			vector<float> CPT = node->get_CPT();

// 			// Create two new records based on the probabilities
// 			for (int i = 0; i < 2; i++) {
// 				// Sample a value based on the CPT
// 				string sampledValue = sample_value(CPT);

// 				// Update the missing value in the record
// 				updatedRecord[missingIndex] = node->get_values()[stoi(sampledValue)];

// 				// Write the updated record to the new dataset file
// 				newDatasetFile << updatedRecord[0];
// 				for (int j = 1; j < updatedRecord.size(); j++) {
// 					newDatasetFile << " " << updatedRecord[j];
// 				}
// 				newDatasetFile << endl;
// 			}
// 		}
// 		else {
//             // If there is no "?", write the original record as-is to the new dataset file
//             newDatasetFile << line << endl;
//         }
//     }

//     // Close the dataset files
//     datasetFile.close();
//     newDatasetFile.close();

// 	if (counter%40==0) evaluate_CPT(Alarm, "new_records.dat") ; 
// }
void EM_step(Dataset& dataset1, Dataset& dataset2, network& Alarm) {
    int missingCounter = 0;
    
    for (const auto& missingValue : dataset1.get_missing_values_positions()) {
        int row = missingValue.first;
        int col = missingValue.second;

        if (col >= dataset1.get_record(row).size()) {
            cerr << "Error: Invalid missing value position." << endl;
            continue;
        }

        // Retrieve the node corresponding to the missing value
        list<Graph_Node>::iterator node = Alarm.get_nth_node(col);
        vector<float> CPT = node->get_CPT();

        // Sample a value based on the CPT
        string sampledValue = sample_value(CPT);

        // Update dataset2 with the sampled value
        dataset2.get_record(row)[col] = node->get_values()[stoi(sampledValue)];

        missingCounter++;

        if (missingCounter % 40 == 0) {
            evaluate_CPT(Alarm, dataset2); // Update CPT table after sampling every 40 missing values
        }
    }
}



int main()
{
	network Alarm;
	Alarm=read_network();
	float maxscore = -1 ; 
	int iterations = 100 ; 
	network best_Alarm = Alarm;
	Dataset dataset1  ; 
	read_data_file("records.dat", dataset1) ; 
	Dataset dataset2 = dataset1  ; 

	while(iterations--){
		random_initialise_data(dataset1, dataset2, Alarm) ; 
		evaluate_CPT(Alarm, dataset2) ; 
		float epsilon = 0.005 ; 
		float delta = 1.0  ; 
		while(delta > epsilon){
			network before = Alarm ; 
			EM_step(dataset1, dataset2, Alarm) ; 
			delta = calc_change(before, Alarm) ; 
		}
		float score = eval_score(Alarm) ; 
		if (score > maxscore) best_Alarm = Alarm ; 
	}

	// output alarm

	// cout<<"Perfect! Hurrah! \n";
	
}





