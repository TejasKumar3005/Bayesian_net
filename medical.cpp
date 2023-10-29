#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
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


void CPT_initialise(network &Alarm) {
    // Loop through each node in the network
	// maybe make this less time expensive by including the list of graph nodes of a network in public
    for (int i = 0; i < Alarm.netSize(); i++) {
        list<Graph_Node>::iterator node = Alarm.get_nth_node(i);
        vector<float> current_CPT = node->get_CPT();
		float uniform_prob = 1.0 / node->get_nvalues();
		for (int j = 0; j < node->get_nvalues(); j++) {
			current_CPT[j] = uniform_prob;
		}
		node->set_CPT(current_CPT);
    }
}


void estimate_dataset(network &Alarm, const string &inputDataset, const string &outputDataset) {
    ifstream input(inputDataset);
    ofstream output(outputDataset);

    if (!input.is_open() || !output.is_open()) {
        cout << "Failed to open input or output dataset file." << endl;
        return;
    }

    string line;
    while (getline(input, line)) {
		// maybe check the following 
        stringstream ss(line);
        vector<string> record;
        string value;
        while (ss >> value) {
            record.push_back(value);
        }

        // Find the index of the node with missing value
        int missingIndex = -1;
        for (int i = 0; i < record.size(); i++) {
            if (record[i] == "?") {
                missingIndex = i;
                break;
            }
        }

        if (missingIndex != -1) {
            list<Graph_Node>::iterator node = Alarm.get_nth_node(missingIndex);
            vector<float> CPT = node->get_CPT();


			// the code below is wrong. what we have to do is calculate P[X=2/given everything] = P[X/given parents]*(for all children Y of X)*P[Y/given parents]
			// note here that number of values X can take can be more than 2 so calculate for all of them. 
			// see the else portion below: also write the weight of each dataset along with dataset 

            // float probMissingIsOne = CPT[node->get_nvalues() - 1]; // Probability of missing value being 1

            // // Create two new records based on the probability
            // for (int i = 0; i < 2; i++) {
            //     vector<string> newRecord = record;
            //     newRecord[missingIndex] = (i == 0) ? "0" : "1";
            //     output << newRecord[0];
            //     for (int j = 1; j < newRecord.size(); j++) {
            //         output << " " << newRecord[j];
            //     }
            //     output << endl;
            // }


        } else {
            // If no missing value, just copy the record to the output dataset
            output << 1.0<<" "<<line << endl;
        }
    }

    input.close();
    output.close();
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

	int iterations = 100 ; 
	network best_Alarm = Alarm;
	while(iterations--){
		CPT_initialise(Alarm) ; 
		double epsilon = 0.05 ; 
		// store old CPT
		while (true){
			estimate_dataset(Alarm, "records.dat", "temp_records.dat");
			maximise_CPT() 
			// double new_max_CPT_change = max change in new and old CPT values
			if (new_max_CPT_change<= epsilon) break ; 
		}
		// evaluate score of this network. if better than previous, update best_Alarm
	}
	write_output() ; 

	// cout<<"Perfect! Hurrah! \n";
	
}


// dnkk





