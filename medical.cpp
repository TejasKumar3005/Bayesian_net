#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
#include <random>
#include <chrono>
#include <algorithm>
#include <ctime>


// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
    vector<int> parents_pos; // index of parents in graph
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

public:
    vector<int> markov_blanket; // Markov blanket of a node- index of nodes in graph
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
    void set_Parents_index(vector<int> Parent_Nodes)
    {
        parents_pos.clear();
        parents_pos=Parent_Nodes;
    }
    vector<int> get_Parents_index()
    {
        return parents_pos;
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

	vector <Graph_Node> Pres_Graph;

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
        for(int i=0;i<Pres_Graph.size();i++)
        {
            if(Pres_Graph[i].get_name().compare(val_name)==0)
                return i;
        }
        return -1;
    }
// get the node at nth index
    Graph_Node get_nth_node(int n)
    {
       if (n<Pres_Graph.size())
           return Pres_Graph[n];
        else
            cout<<"index out of range\n";
    }
    //get the iterator of a node with a given name
    // list<Graph_Node>::iterator search_node(string val_name)
    // {
    //     list<Graph_Node>::iterator listIt;
    //     for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
    //     {
    //         if(listIt->get_name().compare(val_name)==0)
    //             return listIt;
    //     }
    
    //         cout<<"node not found\n";
    //     return listIt;
    // }

    void set_markov_blanket(){
        for (int i = 0; i > Pres_Graph.size(); i++){
            Graph_Node node = Pres_Graph[i];
            vector<int> children = node.get_children();
            vector<string> parents = node.get_Parents();
            unordered_set <int> markov_blanket;
            for (int j = 0; j < children.size(); j++){
                markov_blanket.insert(children[j]);
            }
            for (int j = 0; j < parents.size(); j++){
                markov_blanket.insert(this->get_index(parents[j]));
            }
            // maybe duplicates in the below code, maybe set markov blanket as a set to avoid duplicates and at the end convert it to a vector again to keep in line with the format 
             
            for (int j = 0; j < children.size(); j++){
                Graph_Node child = Pres_Graph[children[j]];
                vector<string> child_parents = child.get_Parents();
                for (int k = 0; k < child_parents.size(); k++){
                    markov_blanket.insert(this->get_index(child_parents[k]));
                }
            }

            vector<int> markov_blanket_vector;
            for (auto it = markov_blanket.begin(); it != markov_blanket.end(); it++){
                markov_blanket_vector.push_back(*it);
            }
            node.markov_blanket = markov_blanket_vector;
        }
    }

	
	float calc_change(network& before, network& after) {
		float maxChange = 0.0;

		Graph_Node* beforeNode;
		Graph_Node* afterNode;

		for (int index = 0 ; index < Pres_Graph.size() ; index++) {
			Graph_Node beforeNode = before.get_nth_node(index) ; 
			Graph_Node afterNode = after.get_nth_node(index) ; 
			vector<float> beforeCPT = beforeNode.get_CPT();
			vector<float> afterCPT = afterNode.get_CPT();

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

    // Reverse map for converting back from integers to values
    vector<vector<string>> value_list;
    vector<unordered_map<string, int>> variable_value_map;   
    vector<vector<vector<int>>> counts; // counts[i][k][j] is the number of times variable i  with parents taking kth configuration takes value j

    // Store the lines and positions of question marks.
    // E.g. If question mark is in line 3 at position 2, we store it as {3, 2}
    vector<pair<int, int>> missing_values_positions;

public:
    // Constructor
    Dataset() {}

    void set_variable_value_map(network& Alarm) {
        for (int i = 0; i < Alarm.netSize(); ++i) {
            Graph_Node& node = Alarm.get_nth_node(i);
            vector<string>& values = node.get_values();
            unordered_map<string, int> value_map;
            for (int j = 0; j < values.size(); ++j) {
                value_map[values[j]] = j;
            }
            variable_value_map.push_back(value_map);
            value_list.push_back(values);
        }
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
                if (variable_value_map[i].find(value) == variable_value_map[i].end()) {
                   cerr << "Error: Invalid value " << value << " for variable " << i << endl;
                }
                int_record.push_back(variable_value_map[i][value]);
            }
        }
        data_matrix.push_back(int_record);
    }

    // Retrieve a specific patient record
    vector<int> get_record(int index) const {
        return data_matrix[index];
    }

    // Retrieve the positions of all missing values
    const vector<pair<int, int>>& get_missing_values_positions() const {
        return missing_values_positions;
    }

    int get_parent_config(vector<int> possible_values, vector<int> parents_values) {
        int config = 0;
        int multiply_further = 1;
        for (int i = 0; i < possible_values.size(); ++i) {
            multiply_further *= possible_values[i];
        }
        for (int i = 0; i < possible_values.size(); ++i) {
            multiply_further /= possible_values[i];
            config += parents_values[i]*multiply_further;
        }
        return config;
    }

    void set_counts(network& Alarm) {
        for (int i = 0; i < data_matrix.size(); ++i) {
            vector<int> record = data_matrix[i];
            for (int j = 0; j < record.size(); ++j) {
                Graph_Node& node = Alarm.get_nth_node(j);
                vector<int> parents_pos = node.get_Parents_index();
                vector<int> parents_values;
                // parents_values.push_back(record[j]);
                vector<int> possible_values;
                // possible_values.push_back(node.get_values().size());
                for (int k = 0; k < parents_pos.size(); ++k) {
                    parents_values.push_back(record[parents_pos[k]]);
                    possible_values.push_back(Alarm.get_nth_node(parents_pos[k]).get_values().size());
                }
                int pos = get_parent_config(possible_values,parents_values);
                if (j >= counts.size()) {
                    // increase the size of counts to j+1
                    counts.resize(j+1);
                }
                if (pos >= counts[j].size()) {
                    // increase the size of counts[j] to pos+1
                    counts[j].resize(pos+1);
                }
                if (record[j] >= counts[j][pos].size()) {
                    // increase the size of counts[j][pos] to record[j]+1
                    counts[j][pos].resize(record[j]+1);
                }
                counts[j][pos][record[j]]++;

            }
        }
    }

    // Other utility functions can be added as needed...
	// check the below fucntion ?
	void update_value(int row, int col, const string& value) {
        if (row < data_matrix.size() && col < data_matrix[row].size()) {
            if (value == "?") {
                data_matrix[row][col] = -1;
            } else {
                if (variable_value_map[col].find(value) == variable_value_map[col].end()) {
                    cerr << "Error: Invalid value " << value << " for variable " << col << endl;
                }
                data_matrix[row][col] = variable_value_map[col][value];
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
     				
                    int pos1;
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        pos1=Alarm.get_index(temp);
                        Alarm.get_nth_node(pos1).add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    vector<int> parents_pos;
                    for (int i = 0; i < values.size(); ++i) {
                        parents_pos.push_back(Alarm.get_index(values[i]));
                    }
                    Alarm.get_nth_node(index).set_Parents_index(parents_pos);
                    Alarm.get_nth_node(index).set_Parents(values);
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
                    
                    Alarm.get_nth_node(index).set_CPT(curr_CPT);

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




// functions used in int main: 
void random_initialise_data(Dataset& dataset1, Dataset& dataset2, network& Alarm) {
    const vector<pair<int, int>>& missing_positions = dataset2.get_missing_values_positions();

    for (const auto& position : missing_positions) {
        int row = position.first;
        int col = position.second;

        Graph_Node node = Alarm.get_nth_node(col);
        vector<string> possibleValues = node.get_values();

        // Randomly select a value from the possible values in dataset1
        int randomIndex = rand() % possibleValues.size();
        string replacementValue = possibleValues[randomIndex];

        // Update the missing value in dataset2
        dataset2.update_value(row, col, replacementValue);
    }
}


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

void evaluate_CPT(network& Alarm, Dataset& dataset) {
    for (int i = 0; i < Alarm.netSize(); ++i) {
        Graph_Node& node = Alarm.get_nth_node(i);
        vector<int> markov_blanket = node.markov_blanket;
        vector<string> values = node.get_values();
        vector<float> CPT(values.size(), 0.0);



        // Update the CPT table
        node.set_CPT(CPT);
    }
}


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
        Graph_Node node = Alarm.get_nth_node(col);
        vector<float> CPT = node.get_CPT();

        // Sample a value based on the CPT
        string sampledValue = sample_value(CPT);

        // Update dataset2 with the sampled value

		// the follwoing seems wrong ?? shouldnt we use update_value function ?
        dataset2.get_record(row)[col] = stoi(sampledValue);

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
    
// Example: to do something

	// 1. initialise CPT values to something(preferably using the dataset) 
	// 2. find the probabilities of ? to be 1 or 0 and then make new dataset
	// 3. calculate new CPT(dont forget smoothing) using this dataset and then delete this dataset
	// 4. repeat step 2, 3 until the max difference in all the CPT values is less than epsilon
	// 5. write the CPT values in the required file 


	// CPT_initialise() ; 
	// double epsilon = 0.05 ; 
	// // store old CPT
	// while (true){
	// 	estimate_dataset() ; 
	// 	maximise_CPT() 
	// 	// double new_max_CPT_change = max change in new and old CPT values
	// 	if (new_max_CPT_change<= epsilon) break ; 
	// }
	// write_output() ; 

	// cout<<"Perfect! Hurrah! \n";


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




	
}


// dnkk




