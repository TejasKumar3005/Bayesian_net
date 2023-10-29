#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cstdlib>


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


	CPT_initialise() ; 
	double epsilon = 0.05 ; 
	// store old CPT
	while (true){
		estimate_dataset() ; 
		maximise_CPT() 
		// double new_max_CPT_change = max change in new and old CPT values
		if (new_max_CPT_change<= epsilon) break ; 
	}
	write_output() ; 

	// cout<<"Perfect! Hurrah! \n";
	
}


// dnkk




