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
float m = 1.0 ; 
// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{

private:

public:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
    vector<int> parents_pos; // index of parents in graph
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
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
	// friend float calc_change(network& before, network& after); // Declare calc_change as a friend


public:
	vector <Graph_Node> Pres_Graph;
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
    
    //    if (n>=Pres_Graph.size()){cerr<<"index out of range\n";}
        
        return Pres_Graph[n] ;
        
            
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

    void set_nth_node(int n, Graph_Node node){
        Pres_Graph[n] = node;
    }



};
	
float calc_change(network& before, network& after) {
    float maxChange = 0.0;

    Graph_Node* beforeNode;
    Graph_Node* afterNode;

    for (int index = 0 ; index < before.Pres_Graph.size() ; index++) {
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



class Dataset {
public:
    // Data matrix: Each row represents a patient record
    // If an entry is -1, it denotes a missing value ('?')
    vector<vector<int>> data_matrix;

    // Reverse map for converting back from integers to values
    // vector<vector<string>> value_list;
    vector<unordered_map<string, int>> variable_value_map;   

    // Store the lines and positions of question marks.
    // E.g. If question mark is in line 3 at position 2, we store it as {3, 2}
    vector<pair<int, int>> missing_values_positions;

    // Constructor
    vector<vector<vector<int>>> counts; // counts[i][k][j] is the number of times variable i  with parents taking kth configuration takes value j
    Dataset() {}

    void set_variable_value_map(network& Alarm) {
        for (int i = 0; i < Alarm.netSize(); ++i) {
            Graph_Node node = Alarm.Pres_Graph[i];
            vector<string> values = node.get_values();
            unordered_map<string, int> value_map;
            for (int j = 0; j < values.size(); ++j) {
                value_map[values[j]] = j;
            }
            variable_value_map.push_back(value_map);
            // value_list.push_back(values);
        }
    }   

    // Add a new patient record
    void add_record(const vector<string>& record) {
        vector<int> int_record;
        for (int i = 0; i < record.size(); ++i) {
            string value = record[i];
            if (value.substr(1,value.size()-2) == "?") {
                // cout << "in ?????" << endl;
                int_record.push_back(-1);
                missing_values_positions.push_back({static_cast<int>(data_matrix.size()), i});
            } else {
                if (i>= variable_value_map.size()){
                    variable_value_map.resize(i+1);
                }
				// cout << value << " hi2" << endl;
                if (variable_value_map[i].find(value) == variable_value_map[i].end()) {
                    // cout all keys of variable_value_map[i]
                    // for (auto it = variable_value_map[i].begin(); it != variable_value_map[i].end(); it++){
                    //     cout << it->first << endl;
                    // }
                    // cout << value << "hi" << endl;
                    // cout << i << endl;

					// ?? acc to me it should never enter this if  
                   cout << "Error: Invalid value " << value << " for variable " << i << endl;
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
            config += parents_values[i]*multiply_further;
            multiply_further /= possible_values[i];
			// check above ??
        }
        return config;
    }

    void set_counts(network& Alarm) {
		// counts.resize(Alarm.Pres_Graph.+1);
        for (int i = 0; i < data_matrix.size(); ++i) {
            vector<int> record = data_matrix[i];
            for (int j = 0; j < record.size(); j++) {
				cout <<"for record "<<i<<endl ; 
                Graph_Node node = Alarm.Pres_Graph[j];
                vector<int> parents_pos = node.get_Parents_index();
                vector<int> parents_values;
                // parents_values.push_back(record[j]);
                vector<int> possible_values;
                // possible_values.push_back(node.get_values().size());
                for (int k = 0; k < parents_pos.size(); ++k) {
                    parents_values.push_back(record[parents_pos[k]]);
					cout <<record[parents_pos[k]]<<" " ; 
                    possible_values.push_back(Alarm.Pres_Graph[parents_pos[k]].get_values().size());
                }
				cout <<endl ; 

                int pos = get_parent_config(possible_values,parents_values);
				int multiply_further = 1;
				for (int i = 0; i < possible_values.size(); ++i) {
					multiply_further *= possible_values[i];
				}
				// counts.push_back(vector<vector<int>> (multiply_further, vector<int>(node.get_values().size() , 0)))  ;
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
				cout <<"increasing count of variable "<<j<<" with config "<<pos<<" and value "<<record[j]<<endl ; 
				vector<int> x = parents_pos; 
				cout <<"hello" ; 
				// for (auto it  : x) cout <<it<<" " ; cout <<endl ;
				cout << "start" << endl;
                auto a = counts[j];
				cout << 1 << endl ;
				auto b = counts[j][pos];
				cout << 2 << endl ;
				counts[j][pos][record[j]]++;
				cout << 34 << endl ;

            }
        }
    }

    // Other utility functions can be added as needed...
	// check the below fucntion ?
	void update_value(int row, int col, int value) {
		data_matrix[row][col] = value;
    }
};


vector<string> split_record(const string& line) {
    // Remove the first and last quote
    string trimmed = line;

    // Split the string by the delimiter '" "'
    vector<string> record;
    size_t start = 0;
    size_t end = trimmed.find(" ");
    
    while (end != string::npos) {
        record.push_back(trimmed.substr(start, end - start));
        start = end + 1; // move past the found delimiter
        end = trimmed.find(" ", start);
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
                    // cout all values
                    for (int i = 0; i < values.size(); ++i) {
                        cout << values[i] << " ";
                    }
					cout<<endl ; 
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
						// cout << temp ;
                        Alarm.Pres_Graph[pos1].add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    vector<int> parents_pos;
					// for (auto i : values) { cout << i << endl;}
					cout << values.size() << endl ;
                    for (int i = 0; i < values.size(); ++i) {
                        parents_pos.push_back(Alarm.get_index(values[i]));
                    }
                    Alarm.Pres_Graph[index].parents_pos = parents_pos;
                    Alarm.Pres_Graph[index].Parents = values;

					vector<int> x = Alarm.Pres_Graph[index].parents_pos ; 
					vector<string> y = Alarm.Pres_Graph[index].Parents;
					cout << "gdi" << endl ;
					for (auto it  : x) cout <<it<<" " ; cout <<endl ;
					cout<<"hiwdh"<<endl;
					for (auto it : y) cout << it << " " ; cout << endl ;
					cout <<"hello" ; 

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
                    
                    Alarm.Pres_Graph[index].set_CPT(curr_CPT);

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

        Graph_Node node = Alarm.Pres_Graph[col];
        vector<string> possibleValues = node.get_values();

        // Randomly select a value from the possible values in dataset1
        int randomIndex = rand() % possibleValues.size();
        string replacementValue = possibleValues[randomIndex];

        // Update the missing value in dataset2
        dataset2.update_value(row, col, randomIndex);
    }
}


int sample_value(const vector<float>& probabilities) {
    if (probabilities.size() == 0) {
        return -1; // Empty probabilities, return an empty string
    }

    // Create a random engine with a seed based on the current time
    unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
    default_random_engine generator(seed);

    // Create a discrete distribution using the probabilities
    discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());

    // Sample a random value
    int sampledIndex = distribution(generator);
    return sampledIndex; // Convert the index to a string
}

void evaluate_CPT(network& Alarm, Dataset& dataset) {
    for (int i = 0; i < Alarm.netSize(); ++i) {
        Graph_Node node = Alarm.Pres_Graph[i];
        vector<string> values = node.get_values();
		string name = node.get_name();
        // cout << "i " << i << endl ;
        // cout << "size " << dataset.counts.size
        vector<vector<int>> counts = dataset.counts[i];
        vector<float> CPT(values.size()*counts.size(), 0.0);
        vector<int> parents_pos = node.get_Parents_index();
		cout << 1 << endl ;
        for (int j = 0; j < counts.size(); j++) {
			cout << 2 << endl ;
            int sum = 0;
            for (int k = 0; k < counts[j].size(); k++) {
				cout << 3 << endl ;
                sum += counts[j][k];
				cout << 4 << endl ;
            }
			cout << 5 << endl ;
            for (int k = 0 ; k < values.size(); k++) {
				// if (name == "\"StrokeVolume\""){
				// 	cout << 
				// }
				cout << 6 << endl ;
				cout << name << endl ;
				cout << values.size() << endl ;
				cout << counts.size() << endl ;
				for (auto i : counts) { for (auto j : i){cout << j << " ";} cout << endl;}
				cout << CPT.size() << endl;
				cout << counts.size()*k + j << endl;
                CPT[counts.size()*k + j] = (counts[j][k] + m) / (sum + m*values.size());
				cout << 7 << endl;
            }
        }
        node.set_CPT(CPT);
        Alarm.set_nth_node(i, node);
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
        Graph_Node node = Alarm.Pres_Graph[col];
        vector<float> CPT = node.get_CPT();

        // Sample a value based on the CPT
        int sampledValue = sample_value(CPT);

        // Update dataset2 with the sampled value

		// the follwoing seems wrong ?? shouldnt we use update_value function ?
        dataset2.update_value(row,col, sampledValue);

        missingCounter++;

        if (missingCounter % 40 == 0) {
			dataset2.set_counts(Alarm);
            evaluate_CPT(Alarm, dataset2); // Update CPT table after sampling every 40 missing values
        }
    }
	dataset2.set_counts(Alarm);
	evaluate_CPT(Alarm, dataset2); 
}






int main()
{

	network Alarm;
	Alarm=read_network();
	vector<int> x = Alarm.Pres_Graph[1].get_Parents_index() ; 
		vector<string> y = Alarm.Pres_Graph[1].get_Parents();
		cout <<"hello" ; 
		for (auto it  : x) cout <<it<<" " ; cout <<endl ;
		for (auto it : y) cout << it << " " ; cout << endl ;
	// return 0;
	float maxscore = -1 ; 
	int iterations = 1 ; 
	network best_Alarm = Alarm;
	Dataset dataset1; 
    dataset1.set_variable_value_map(Alarm);
	read_data_file("records.dat", dataset1) ; 
	Dataset dataset2 = dataset1  ; 

	while(iterations--){
		random_initialise_data(dataset1, dataset2, Alarm) ; 
        dataset2.set_counts(Alarm);
        cout << "done" << endl ;
		evaluate_CPT(Alarm, dataset2) ;
		cout<< "khatam bc" << endl; 
		float epsilon = 0.001 ; 
		float delta = 1.0; 
		while(delta > epsilon){
			network before = Alarm ; 
			EM_step(dataset1, dataset2, Alarm) ; 
			delta = calc_change(before, Alarm) ; 
			cout <<delta<<endl ; 
		}
		// float score = eval_score(Alarm) ; 
		// if (score > maxscore) best_Alarm = Alarm ; 
        // cout<<"iteration "<<iterations<<endl;
        // cout<<"delta "<<delta<<endl;
		// cout << Alarm.Pres_Graph[1).get_name() <<endl; 
		// cout <<"matrix printing sadc"<<endl ; 
		vector<int> x = Alarm.Pres_Graph[1].get_Parents_index() ; 
		vector<string> y = Alarm.Pres_Graph[1].get_Parents();
		cout <<"hello" ; 
		for (auto it  : x) cout <<it<<" " ; cout <<endl ;
		for (auto it : y) cout << it << " " ; cout << endl ;
 		cout <<"hello" ; 
		for(int i = 0 ; i < dataset2.counts[1].size() ; i++){
			for(int j = 0 ; j < dataset2.counts[1][i].size() ; j++) cout <<dataset2.counts[1][i][j]<<" " ; 
			cout <<endl ; 
		}
		// // cout <<endl ; 
		// cout <<"hello" <<endl ; 
        // for(int i = 0 ; i < Alarm.Pres_Graph[1).get_CPT().size(); i++) cout<< Alarm.Pres_Graph[1).get_CPT()[i]<<" ";
	}
}