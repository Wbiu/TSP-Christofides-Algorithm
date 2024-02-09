#include <iostream>
#include <fstream>
#include <document.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
//#include <random>
#include <algorithm>
#define MAX_EDGE_WEIGHT 200
#define MARKED_ZERO 88 // 88 => 'X'

struct Edge;

struct Vertex
{
	const char* name;
	std::vector<Edge> edges;
};

struct Edge
{
	int weight;
	Vertex* vertex_p_1;
	Vertex* vertex_p_2;	
	char identifier[2];
	void identifying() {
		if( (int)*vertex_p_1->name  < (int)*vertex_p_2->name )
		{
			identifier[0] = (char)*vertex_p_1->name;
			identifier[1] = (char)*vertex_p_2->name;
		}
		else
		{
			identifier[1] = (char)*vertex_p_1->name;
			identifier[0] = (char)*vertex_p_2->name;
		}
	}
};

struct kGroup
{
	int groupID = 0;
	std::unordered_map < std::string, Edge> member;
};

enum LineType
{
	HORIZONTAL,
	VERTICAL
};

struct Line
{
	LineType lt;
	std::string vName;
	int atIdxRow;
	int atIdxCol;
};

struct Pair
{	
	std::string vName;
	std::vector<int> potentialPairs;

};

std::pair<char,int>* findMin(std::unordered_map<char,int> map)
{
	std::pair<char, int> curPair, prevPair = *map.begin();

	//std::pair<char, int> prePair; what is this data typ ?????
	
	if (prevPair.second == 0)
	{
		for (auto& KV : map)
		{
			if (KV.second > prevPair.second)
			{
				prevPair = KV;
				break;
			}
		}
	}
	
	for (std::pair<char,int> KV : map)
	{
		curPair = KV;
		if (curPair.second < prevPair.second && curPair.second != 0 && prevPair.second != 0)
		{
			prevPair = curPair;		
		}
	}
	return &prevPair;
}

void printAdjMx(const std::vector<std::vector<int>> adjMx)
{
	for (int i = 0; i < adjMx.size();i++)
	{
		for (int j = 0; j < adjMx[i].size();j++)
		{
			if (adjMx[i][j] >= (int)*"A"  && adjMx[i][j] <= (int)*"Z")
			{
				std::cout << (char)adjMx[i][j] << "|";
			}
			else
			{
				std::cout << adjMx[i][j] << "|";
			}
		}
		std::cout << "\n";
	}
	
	std::cout << "\n";

}
void printAdjMxAsText(std::vector<std::vector<int>> adjMx)
{
	for (int i = 1; i < adjMx.size();i++)
	{
		for (int j = 1; j < adjMx[i].size();j++)
		{

			/*
			if (adjMx[i][j] >= (int)*"A"  && adjMx[i][j] <= (int)*"Z")
			{
				std::cout << (char)adjMx[i][j];
			}
			else
			{
				std::cout << adjMx[i][j] << "|";
			}*/

			std::cout << (char) adjMx[i][0] << " -> "  << (char) adjMx[0][j] << " weight : " << adjMx[i][j] << "\n";



		}
		std::cout << "\n";
	}
	


}
bool cmp(Edge e1,Edge e2)
{
	if (e1.weight < e2.weight)
		return 1;
	else
		return 0;
}

// attempts to create Hamiltonian_path algorithm
const int N = 5;
bool Hamiltonian_path(
	std::vector<std::vector<int> >& adj, int N)
{
	int* adr = &N;
	*adr = N;

	int dp[::N][(1 << ::N)];

	// Initialize the table
	memset(dp, 0, sizeof dp);

	// Set all dp[i][(1 << i)] to
	// true
	for(int i = 0; i < N; i++)
		dp[i][(1 << i)] = true;

	// Iterate over each subset
	// of nodes
	for(int i = 0; i < (1 << N); i++) {

		for(int j = 0; j < N; j++) {

			// If the jth nodes is included
			// in the current subset
			if(i & (1 << j)) {

				// Find K, neighbour of j
				// also present in the
				// current subset
				for(int k = 0; k < N; k++) {

					if(i & (1 << k)
						&& adj[k][j]
						&& j != k
						&& dp[k][i ^ (1 << j)]) {

						// Update dp[j][i]
						// to true
						dp[j][i] = true;
						break;
					}
				}
			}
		}
	}

	// Traverse the vertices
	for(int i = 0; i < N; i++) {

		// Hamiltonian Path exists
		if(dp[i][(1 << N) - 1])
			return true;
	}

	// Otherwise, return false
	return false;
}


int main()
{	


	//read file
	char file_buff;
	std::string fileContent;
	int nodeCount = 0;
	std::unordered_map<std::string, Vertex> verteicesMap_p;
	std::ifstream file("area.json");
	std::cout << "read area file\n";
	if(file.is_open())
	{
		while (file)
		{
			file_buff = file.get();
			if ((int)file_buff != -1) // -1 end of file,
			{
				fileContent += file_buff;
			}
			else
			{
				fileContent += '\0'; // replace -1/ terminates the string
			}
		}
	}
	file.close(); // closing the file
	//check if file contains anything 
	if(fileContent.size() == 0)
	{
		std::cerr << "No file content! or file not found !\n";
		std::cerr << "Press enter to exit\n";
		std::cin.get();
		exit(0);
	}

	const char* json = fileContent.c_str();
	//end read file


	//Decode json string
	rapidjson::Document rootDoc;
	rootDoc.Parse(json);
	rapidjson::Value& rootValue = rootDoc["area"];
	std::cout << "Informations" << std::endl;
	std::cout << "Area name : " << rootValue["name"].GetString() << std::endl;
	rapidjson::Value& nodesValue =  rootValue["nodes"];

	assert(nodesValue.IsArray());

	// create vertex and put them to the vetices map
	for (rapidjson::SizeType i = 0; i < nodesValue.Size(); i++)
	{
		rapidjson::Value& subNode = nodesValue[i];
		const char* c = subNode["node"].GetString();
		Vertex vertex{
			c
		};
		verteicesMap_p[c] = vertex;
		nodeCount++;
	}
	
	//Create Edges 
	std::vector<Edge> edgeList; // for Kruskal's Algorithm
	std::unordered_map<std::string, Edge> edgeMap;
	
	for (rapidjson::SizeType i = 0; i < nodesValue.Size(); i++)
	{
		rapidjson::Value& subNode = nodesValue[i];
		rapidjson::Value& links = subNode["links"];
		const char* c = subNode["node"].GetString();

		for (rapidjson::SizeType j = 0; j < links.Size(); j++)
		{
			rapidjson::Value link = links[j].GetObject();
			for (rapidjson::Value::ConstMemberIterator itr = link.MemberBegin(); itr != link.MemberEnd(); ++itr)
			{
				Edge e{
					std::atoi(itr->value.GetString()),
					&verteicesMap_p.find(c) -> second, 
					&verteicesMap_p.find(itr->name.GetString())->second
				};
				e.identifying();
				
				// = e.identifying();
				//const char* n1 = verteicesMap_p.find(c)->second.name;
				//const char* n2 = verteicesMap_p.find(itr->name.GetString())->second.name;
				//int identifier = (int)*n1 + (int)*n2;
				//	e.identifier


				verteicesMap_p[c].edges.push_back(e);

				//use a map to filter out dopplicates
				std::string key =  e.identifier;
				edgeMap[key] = e;

	//			edgeList.push_back(e);
			//	std::cout << itr->name.GetString() << " : " << itr->value.GetString() << std::endl;
			}
		}
	}

	if(nodeCount > 26)
	{
		std::cerr << "To many nodes !!  More then 26 \n";
		std::cerr << "Press [Enter] to exit\n";
		std::cin.get();
		exit(0);
	}
	std::cout << "Node count : " << nodeCount << std::endl;

	// adjacency matrix
	++nodeCount; //add one more row & col !!


	int num_col = nodeCount;
	int num_row = nodeCount;
	std::vector<int> colums(num_col);

	std::vector<std::vector<int>> adj_mx(num_row,colums); 


	// add row and collow name to the adjecency matrix
	/*
	 0 | A | B | C ...
	 A | 0 | 0 | 0 ...
	 B | 0 | 0 | 0 ...
	 C | 0 | 0 | 0 ...
	*/

	/*
	*another way to create the adjacency matrix
	Use container struct to store Vertices and Edges:
	
	enum containerType {vertex,egde,...}

	struct container
	{
		containerTye = vertex,
		Vertex* vertex = nullptr,
		Egde edge = nullptr
	};

	std::vector<container> ...

	*/

	int col = 0;
	int row = 0;
	std::unordered_map<std::string, int> idx_vertices; 
	for (auto& KV : verteicesMap_p)
	{	
		Vertex v = KV.second;
		adj_mx[0][++col] = (int)*v.name;
		idx_vertices[v.name] = col; // storing the indecies of vertices int the adjacency matrix.

		if(row != (adj_mx.size() - 1))
		{
			adj_mx[++row][0] = (int)*v.name;
		}
		else
		{
			adj_mx[row][0] = (int)*v.name;
		}
	}

	// creating a empty copie of the adjecency matrix for MST
	std::vector<std::vector<int>> mst_adj_mx = adj_mx;

	// filling the adjecency matrix with weights	
	for (auto& KV : verteicesMap_p)
	{
		Vertex v = KV.second;
		int idx_1 = (int)*v.name;
		int idx_loc_1 = 0, idx_loc_2 = 0;

		for (int i = 0; i < v.edges.size();i++)
		{	
			Edge e = v.edges[i];
			int weight = e.weight;
			int idx_2 = (int)*e.vertex_p_2->name;

			for (int j = 1; j < adj_mx.size();j++)
			{
				if (adj_mx[j][0] == idx_1)
				{
					idx_loc_1 = j;
					break;
				}

			}

			for (int k = 1; k < adj_mx.size();k++)
			{
				if (adj_mx[0][k] == idx_2)
				{
					idx_loc_2 = k;
				}
			}
			adj_mx[idx_loc_1][idx_loc_2] = weight;

		}

	}
	std::cout << "adjacency matrix \n";
	printAdjMx(adj_mx);

	//from map to vector
	for(auto& kv : edgeMap)
	{
	//	std::cout << kv.first << " " << kv.second.identifier << "\n"
		edgeList.push_back(kv.second);
	}


	//Begin kruskals algorythm
	std::cout << "------------------[Begin Kruskal's algorithm]-----------------\n";
	//Sort the edges
	std::sort(edgeList.begin(), edgeList.end(), cmp);

	//List all edges
	std::cout << "List of all Edges ( Sorted edges ) \n";
	for(Edge e : edgeList)
	{
		std::cout << e.identifier << " " << e.weight << "\n";
	}
	std::cout << "\n";

	int grpID = 0;
	std::vector<Edge> mstEdgeList;
	std::unordered_map<std::string, int> vertexInGroup;
	std::unordered_multimap<int,std::string> vertexInGroup_Milti;
//	std::multimap<int,std::string> vertexInGroup_Milti;
	
	std::cout << "Creating MST: \n";
	
	//the main algorithm
	for(Edge e : edgeList)
	{
		bool createNewGrpID = false;
		std::string v1_name = e.vertex_p_1->name;
		std::string v2_name = e.vertex_p_2->name;
		const char* ident = e.identifier;
		

		if(vertexInGroup.count(v1_name) > 0 && vertexInGroup.count(v2_name) > 0)
		//if(vertexInGroup_Milti.count(v1_name) > 0 && vertexInGroup_Milti.count(v2_name) > 0)
		{

			if(vertexInGroup[v1_name] == vertexInGroup[v2_name])
		//	if(vertexInGroup_Milti[v1_name] == vertexInGroup_Milti[v2_name])
			{
				std::cout << "loop ? " << v1_name << " is in grp : " << vertexInGroup[v1_name] << " " << v2_name << " is in group : " << vertexInGroup[v2_name] << "\n";
				continue;
			}

			//merge group
			if(vertexInGroup[v1_name] != vertexInGroup[v2_name])
			{
				
				std::string edgeName = v1_name < v2_name ? v1_name + v2_name : v2_name + v1_name;
				int target1 = vertexInGroup[v1_name];
				int target2 = vertexInGroup[v2_name];

				std::cout << "merging group " << target2 << " to " << target1 << "\n";
				auto range1 = vertexInGroup_Milti.equal_range(target1);
				auto range2 = vertexInGroup_Milti.equal_range(target2);
				
				for(auto itr = range1.first; itr != range1.second; ++itr)
				{	
					vertexInGroup_Milti.insert({vertexInGroup[v2_name],itr->second});
					vertexInGroup[itr->second] = target2;

				}
				vertexInGroup_Milti.erase(target1);
				
				mstEdgeList.push_back(edgeMap[edgeName]);
			}

		}


		if(vertexInGroup.count(v1_name) > 0 && vertexInGroup.count(v2_name) == 0)
		{
			int v1_grpID = vertexInGroup[v1_name];
			vertexInGroup[v2_name] = v1_grpID;

			vertexInGroup_Milti.insert({ v1_grpID,v2_name });
			//std::cout <<"v1 is in grp\n";

				mstEdgeList.push_back(e);
		}

		if(vertexInGroup.count(v1_name) == 0 && vertexInGroup.count(v2_name) > 0)
		{
			int v2_grpID = vertexInGroup[v2_name];
			vertexInGroup[v1_name] = v2_grpID;

			vertexInGroup_Milti.insert({v2_grpID,v1_name });
			
				mstEdgeList.push_back(e);

			//std::cout <<"v2 is in grp\n";
		}

		if(vertexInGroup.count(v1_name) == 0 && vertexInGroup.count(v2_name) == 0)
		{
			vertexInGroup[v1_name] = grpID;
			vertexInGroup[v2_name] = grpID;
			std::cout << "new group " << e.identifier << " " << e.weight << " in groupID : " << grpID << "\n";
			createNewGrpID = true;
			vertexInGroup_Milti.insert({ grpID,v1_name });
			vertexInGroup_Milti.insert({ grpID,v2_name });

				mstEdgeList.push_back(e);
		}
	
		if(createNewGrpID)
		{	

			//std::unordered_map<std::string, int> vertexInGroupTmp = vertexInGroup;
			//kGList.push_back(vertexInGroupTmp);
		//	vertexInGroup.clear();
			grpID++;
			createNewGrpID = false;
		}


	}



	// filling MST adjacency matrix 
	// sum the total cost of the MST
	int totalWeight = 0;
	for(Edge e : mstEdgeList)
	{	
		int idx1 = idx_vertices[e.vertex_p_1->name];
		int idx2 = idx_vertices[e.vertex_p_2->name];
		
		std::cout << "MST matrix creation : " << " edge : " << e.vertex_p_1->name << " -> " << e.vertex_p_2->name << "\n";

		mst_adj_mx[idx1][idx2] = e.weight;
		mst_adj_mx[idx2][idx1] = e.weight;
		
		totalWeight += e.weight;
	
	}

	std::cout << "Total weight : " << totalWeight << "\n";

	std::cout << "Minimum spaning tree adjacency matrix \n";
	printAdjMx(mst_adj_mx);
	
	std::cout << "------------------[end Kruskal's algorithm]-----------------\n";

	std::cout << "\n";
	
	
	//Kuhn-Munkres Algorithm (Hungarian Algorithm)
	// BEGIN
	
	std::cout << "------------------[Begin Hungarian algorithm]-----------------\n";

	// find odd degree in MST
	std::cout << "Identifying even and odd-degrees : \n";

//	std::unordered_map<std::string,Vertex> mapOfOddDegrees;
	std::map<std::string,Vertex> mapOfOddDegrees; //changed to a normal map !!
	for(int i = 1; i < mst_adj_mx.size(); i++)
	{
		int numOfEdges = 0;

		for(int j = 1;j < mst_adj_mx.size(); j++)
		{
			if(mst_adj_mx[i][j] > 0)
			{
				numOfEdges++;
			}
		}

		if(numOfEdges % 2)
		{
			std::cout << (char) mst_adj_mx[i][0] << " is odd \n";
			char vCharName = (char) mst_adj_mx[i][0];
			std::string vName = &vCharName;
			mapOfOddDegrees[vName] = verteicesMap_p[vName];

		}
		else
		{
			std::cout << (char) mst_adj_mx[i][0] << " even odd \n";
		}
		numOfEdges = 0;

	}

	std::cout<< "\n";

	// construct subgraph
	std::vector<Vertex> subgraphOddDegrees;

	for(auto& kv : mapOfOddDegrees)
	{
		Vertex* v = &kv.second;
		
		std::vector<Edge> subEdgeList;

		for(Edge e : v->edges)
		{
			//if(mapOfOddDegrees.count())
			if(*e.vertex_p_1->name != *v->name)
			{
				if(mapOfOddDegrees.count(e.vertex_p_1->name) > 0)
				{
					subEdgeList.push_back(e);
				}
			}
			
			if(*e.vertex_p_2->name != *v->name)
			{
				if(mapOfOddDegrees.count(e.vertex_p_2->name) > 0)
				{
					subEdgeList.push_back(e);
				}
			}
		}

		Vertex subV
		{
			v->name,
			subEdgeList
		};
		subgraphOddDegrees.push_back(subV);

	}
	
	//adjecency subgraph matrix
	int subgraphCol = subgraphOddDegrees.size() + 1;
	int subgraphRow = subgraphOddDegrees.size() + 1;

	std::vector<int> subgraphMxCol(subgraphCol);
	std::vector<std::vector<int>> subgraphMx(subgraphRow,subgraphMxCol);
	std::unordered_map<std::string, int> subgraphIdxMap;
	
	//prefillung the Vector names
	for(int i = 0; i < subgraphOddDegrees.size();i++)
	{

		if(i != subgraphMx.size())
		{
			subgraphMx[0][i+1] = *subgraphOddDegrees[i].name;
			subgraphMx[i+1][0] = *subgraphOddDegrees[i].name;

			//pre set weight to be max weight, this means pairs are not optimal at first
			for(int j = 1; j < subgraphMx.size();j++)
			{
				subgraphMx[i+1][j] = MAX_EDGE_WEIGHT;
			}

			subgraphIdxMap[subgraphOddDegrees[i].name] = i+1;

		}
	}

	//filling the real weight subgraph odd-dgree
	for(int i = 0 ; i<subgraphOddDegrees.size();i++)
	{
		Vertex& v = subgraphOddDegrees[i];
		for(Edge& e : v.edges)
		{
			int idx_v_row = subgraphIdxMap[e.vertex_p_1->name];
			int idx_v_col = subgraphIdxMap[e.vertex_p_2->name];
			
			subgraphMx[idx_v_row][idx_v_col] = e.weight;
			subgraphMx[idx_v_col][idx_v_row] = e.weight;
			
		//	subgraphMx[idx_v][] = e.weight;
		}
	}


	std::cout << "Hungarian matrix preview: \n";
	printAdjMx(subgraphMx);

	int numberOfLine = 0;

	//moved to here 
	std::unordered_map<std::string, Edge> pairedEdgesMap;
	std::map<std::string, Vertex>  availableVerticesMap = mapOfOddDegrees;
	bool isMapSizeZero = false;
	while(numberOfLine < subgraphOddDegrees.size() || availableVerticesMap.size() != 0)
	{


		//min row

		std::cout << "Subtrac min row: \n";

		std::vector<int> v_cpy;
		for(int i = 1; i < subgraphMx.size();i++)
		{
			v_cpy = subgraphMx[i];
			int minRow = *std::min_element(v_cpy.begin(), v_cpy.end());
			std::cout << "min row : " << minRow << "\n";
			for(int j = 1; j < subgraphMx.size();j++)
			{
				int weight = subgraphMx[i][j];
				subgraphMx[i][j] = weight - minRow;
			}
		}

		printAdjMx(subgraphMx);


		//min col

		std::cout << "Subtrac min col: \n";
		std::vector<int> v_colMin;
		for(int i = 1; i < subgraphMx.size(); i++)
		{
			//collect colums
			for(int j = 1; j < subgraphMx.size();j++)
			{
				v_colMin.push_back(subgraphMx[j][i]);
			}

			//find min
			auto minCol = *std::min_element(v_colMin.begin(), v_colMin.end());
			std::cout << "min colum : " << minCol << "\n";

			for(int j = 1; j < subgraphMx.size();j++)
			{
				subgraphMx[j][i] = subgraphMx[j][i] - minCol;
			}
			v_colMin.clear();

		}


		printAdjMx(subgraphMx);


		//step 3 cover all zeros
		std::cout << "cover all zeros with minimum number of lines \n";
		std::vector<std::vector<int>> stepThreegraph = subgraphMx;
		std::unordered_map<std::string, Line>  lineLocationCol;
		std::unordered_map<std::string, Line>  lineLocationRow;
		for(int i = 1; i < stepThreegraph.size();i++)
		{
			int zeroEncontered = 0;

			int idxZeroEncounteredRow = -1;
			int idxZeroEncounteredCol = -1;

			for(int j = 1; j < stepThreegraph.size();j++)
			{

				if(stepThreegraph[i][j] == 0)
				{
					zeroEncontered++;
					idxZeroEncounteredRow = i;
					idxZeroEncounteredCol = j;
				}

				if(j == stepThreegraph.size() - 1)
				{
					// if one zero if found in a row 
					// draw a vertical line
					if(zeroEncontered == 1)
					{
						char c  = (char)stepThreegraph[0][idxZeroEncounteredCol];
						std::string sColName = &c;
						if(lineLocationCol.count(sColName) == 0)
						{
							Line l{
								LineType::VERTICAL,
								sColName,
								0, // Locaiton row
								j  // Location col
							};

							lineLocationCol[sColName] = l;
						}
					}

					//if more then 1 zero then draw a horizontal line
					if(zeroEncontered > 1)
					{
					//	char* rowName = new char;
						char r = (char)stepThreegraph[i][0];
						//*rowName = r;
						std::string sRowName = &r;



						if(lineLocationRow.count(sRowName) == 0)
						{
							Line l{
								LineType::HORIZONTAL,
								sRowName,
								i, // location row
								0  // location col
							};
							lineLocationRow[sRowName] = l;

						}

					}
				}
			}
		}

		// if the number of line are equal the number of vertices then the next step can be skiped 
		// and just go straight to the pairing process !!!!
	//	int numberOfLine = lineLocationCol.size() + lineLocationRow.size();

		//numberOfLine = lineLocationColLast.size() + lineLocationRowLast.size();

		// find a minimum number that does not get crossed by any line
		std::vector<int> listOfUncoveredNumber;
		for(int i = 1; i < stepThreegraph.size(); i++)
		{
			for(int j = 1;j < stepThreegraph.size();j++)
			{
				char curRowName = stepThreegraph[i][0];
				char curColName = stepThreegraph[0][j];
				bool inCol = lineLocationCol.count(&curColName) == 0;
				bool inRow = lineLocationRow.count(&curRowName) == 0;

				if(lineLocationCol.count(&curColName) == 0 && lineLocationRow.count(&curRowName) == 0)
				{
					listOfUncoveredNumber.push_back(stepThreegraph[i][j]);
				}

			}
		}

		int minNum = *std::min_element(listOfUncoveredNumber.begin(), listOfUncoveredNumber.end());

		//add the minimum number to the once that got crossed by horizontal and vertical
		std::vector<int> listOfCrossedNumbers;

		std::cout << "add the minimum number [ " << minNum << " ] to the once that got crossed by horizontal and vertical line\n";
		for(auto& kv : lineLocationRow)
		{
			Line l = kv.second;
			std::string sName = l.vName;
			int idxR = subgraphIdxMap[sName];

			for(auto& kv_C : lineLocationCol)
			{
				Line lc = kv_C.second;
				int idxC = subgraphIdxMap[lc.vName];

				int val = stepThreegraph[idxR][idxC];
				val = val + minNum;
				stepThreegraph[idxR][idxC] = val;
			}

		}
		printAdjMx(stepThreegraph);


		//subtrac the minimum number to number that does not cross any line

		std::cout << "subtrac the minimum number [ " << minNum << " ] to those that does not get cross by any lines\n";
		for(int i = 1; i < stepThreegraph.size();i++)
		{
			for(int j = 1; j < stepThreegraph.size();j++)
			{
				char rName = stepThreegraph[i][0];
				char cName = stepThreegraph[0][j];

				bool b1 = lineLocationRow.count(&rName) == 0;
				bool b2 = lineLocationCol.count(&cName) == 0;

				if(lineLocationCol.count(&cName) == 0 && lineLocationRow.count(&rName) == 0)
				{
					stepThreegraph[i][j] = stepThreegraph[i][j] - minNum;
				}
			}
		}

		printAdjMx(stepThreegraph);

		//Last step from step 3 check if number of lines are the same as the alowcation
		std::unordered_map<std::string, Line>  lineLocationColLast;
		std::unordered_map<std::string, Line>  lineLocationRowLast;
		for(int i = 1; i < stepThreegraph.size();i++)
		{
			int zeroEncontered = 0;

			int idxZeroEncounteredRow = -1;
			int idxZeroEncounteredCol = -1;

			for(int j = 1; j < stepThreegraph.size();j++)
			{

				if(stepThreegraph[i][j] == 0)
				{
					zeroEncontered++;
					idxZeroEncounteredRow = i;
					idxZeroEncounteredCol = j;
				}

				if(j == stepThreegraph.size() - 1)
				{
					// if one zero if found in a row 
					// draw a vertical line
					if(zeroEncontered == 1)
					{
						char colName = (char)stepThreegraph[0][idxZeroEncounteredCol];

						if(lineLocationColLast.count(&colName) == 0)
						{
							Line l{
								LineType::VERTICAL,
								&colName,
								0, // Locaiton row
								j  // Location col
							};

							lineLocationColLast[&colName] = l;
						}
					}

					//if more then 1 zero then draw a horizontal line
					if(zeroEncontered >= 2)
					{
						char rowName = (char)stepThreegraph[i][0];
						if(lineLocationRowLast.count(&rowName) == 0)
						{
							Line l{
								LineType::HORIZONTAL,
								&rowName,
								i, // location row
								0  // location col
							};
							lineLocationRowLast[&rowName] = l;

						}

					}
				}
			}
		}

		numberOfLine = lineLocationColLast.size() + lineLocationRowLast.size();

		bool hungarianSuccess = false;
		if(numberOfLine != subgraphOddDegrees.size())
		{
			std::cout << "FAIL !!!!!!!!!!!!!!!!!!\n";
			std::cout << "number of lines does not match to number of required pair\n";
			std::cout << "recalculate Hungarial matrix again !\n";
			std::cout << "\n";
		}
		else
		{
			std::cout << " Hungarina success !! \n";
			std::cout << "\n";
			hungarianSuccess = true;

		}

		subgraphMx = stepThreegraph;

		printAdjMx(subgraphMx);

		//find min pair NEW !!!

		if(hungarianSuccess)
		{
			unsigned int numOfOddPairs = subgraphOddDegrees.size() / 2;

			//	std::unordered_map<std::string, Vertex>  availableVerticesMap = mapOfOddDegrees;

	//		std::map<std::string, Vertex>  availableVerticesMap = mapOfOddDegrees; --> Is now on top of the while loop !!!
			int foundPair = 0;
			//		std::unordered_map<std::string, Edge> pairedEdgesMap; --> Is now on top of the while loop !!!!

			std::unordered_multimap<int, Pair> mapOfPairs;

			//unsing the effect of a map so a group can only exist once
			std::unordered_map<int, int> existingPairGroupMap;
			std::vector<int> existingPairGroupList;
			for(int i = 1; i < subgraphMx.size();i++)
			{

				int numberOfZero = 0; // the key of the maps
				Pair p;
				for(int j = 0; j < subgraphMx.size();j++)
				{
					int val = subgraphMx[i][j];
					if(val == 0)
					{
						numberOfZero++;
						p.potentialPairs.push_back(subgraphMx[0][j]);
					}
				}
				char vName = (char)subgraphMx[i][0];

				p.vName = vName;
				mapOfPairs.insert({ numberOfZero,p });
				existingPairGroupMap.insert({ numberOfZero,numberOfZero });
				numberOfZero = 0;

			}

			for(auto& kv : existingPairGroupMap)
			{
				existingPairGroupList.push_back(kv.first);
			}

			std::sort(existingPairGroupList.begin(), existingPairGroupList.end());


			//	std::unordered_map<std::string, Edge> pairedEdgesMap;
			//	std::unordered_map<std::string, Vertex>  availableVerticesMap = mapOfOddDegrees;
			for(int i = 0; i < existingPairGroupList.size();i++)
			{
				int grpID = existingPairGroupList[i];
				auto range = mapOfPairs.equal_range(grpID);
				for(auto itr = range.first; itr != range.second; ++itr)
				{
					Pair p = itr->second;

					int idxOfVR = subgraphIdxMap[p.vName];
					for(int j = 1; j < subgraphMx.size();j++)
					{
						int val = subgraphMx[idxOfVR][j];
						if(val == 0)
						{

							char cColName = subgraphMx[idxOfVR][0];
							char cRowName = subgraphMx[0][j];
							std::string sColName = &cColName;
							std::string sRowName = &cRowName;
							std::string edgeIdentifier = (int)subgraphMx[idxOfVR][0] > (int)subgraphMx[0][j] ? sRowName + sColName : sColName + sRowName;


							if(availableVerticesMap.count(sColName) > 0 &&
								availableVerticesMap.count(sRowName) > 0)
							{
								if(edgeMap.count(edgeIdentifier) > 0)
								{
									pairedEdgesMap[edgeIdentifier] = edgeMap[edgeIdentifier];
									std::cout << "Pair : " << sColName << " with " << sRowName << "\n";
									availableVerticesMap.erase(sColName);
									availableVerticesMap.erase(sRowName);
									foundPair++;
								}
							}
						}
					}

				}

				std::cout << "";

			}

			if(availableVerticesMap.size() != 0)
			{
				std::cout << "Couldnt pair up all vertices!!! redo Hungarian again !!!!\n ";
				availableVerticesMap = mapOfOddDegrees;
				pairedEdgesMap.clear();
			}
			else
			{
				hungarianSuccess = true;
			}
			isMapSizeZero = availableVerticesMap.size() != 0;
		}

	}
	// end while loop

	//create multigraph/unified graph ( MST + munimum matching pair ) 
	// mst to multigraph
	std::vector<std::vector<int>> multigraph = mst_adj_mx;
	for(auto& kv : pairedEdgesMap)
	{
		Edge e = kv.second;
		int idxP1 = idx_vertices[e.vertex_p_1->name];
		int idxP2 = idx_vertices[e.vertex_p_2->name];

		if(multigraph[idxP1][idxP2] != 0 &&
			multigraph[idxP2][idxP1] != 0)
		{
			multigraph[idxP1][idxP2] = e.weight * 2;
			multigraph[idxP2][idxP1] = e.weight * 2;
	
		}
		else
		{
			multigraph[idxP1][idxP2] = e.weight;
			multigraph[idxP2][idxP1] = e.weight;

		}
		std::cout << "\n";
	}
	
	std::cout << "Multigraph\n";
	printAdjMx(multigraph);
//	printAdjMxAsText(multigraph);

	std::cout << "------------------[End Hungarian algorithm]-----------------\n";
	// END Hungerial algorithm

	std::cout << "\n";


	std::cout << "-----------------[Begin Eulerian Tour]-------------------\n";
	//eulerian tour
	
	
	std::vector<std::string> eTour;
	//add starting vector
	char firstVertex = multigraph[1][0];
	eTour.push_back(&firstVertex);

	bool doubleWeightedEgde = false;
	bool returnToPrevVertex = false;
	int nextVertexIdx = 1;
	char nextVertexname = firstVertex;
	bool doubleWeightedEdgeAtStart = false;
//	std::unordered_map<std::string, Vertex> unvisitedVertices;

	// add one to return from the last vertex to the starting vertex
	bool isStartingVertex = false;

	while(!isStartingVertex)
	{
		

		char curR = multigraph[nextVertexIdx][0];

		int curWeight = -1;
		int idxReplaceWeightC = 0, idxReplaceWeightR = 0;
		int idxLowestVertexWeight = 0;

		for(int j = 1; j < multigraph.size();j++)
		{
			int weightMtg = multigraph[nextVertexIdx][j];
			int weightMst = mst_adj_mx[nextVertexIdx][j];
			
			char curR = multigraph[nextVertexIdx][0];
			char curC = multigraph[0][j];



			if(weightMtg != 0)
			{
				doubleWeightedEgde = (weightMtg / 2) == weightMst ? true : false;
				
				//double weighted edges are prioritize first!!
				if(doubleWeightedEgde)
				{
					multigraph[nextVertexIdx][j] = weightMtg - weightMst;
					multigraph[j][nextVertexIdx] = weightMtg - weightMst;

					nextVertexIdx = j;
					nextVertexname = multigraph[0][j];
					returnToPrevVertex = true;
					
					break;
				}
				else
				{
					if(curWeight == -1 or weightMtg < curWeight)
					{

						idxLowestVertexWeight = j;
						curWeight = weightMtg;

						idxReplaceWeightC = j;
						idxReplaceWeightR = nextVertexIdx;



						if(returnToPrevVertex)
						{
						
							nextVertexname = multigraph[0][idxLowestVertexWeight];
							nextVertexIdx = idxLowestVertexWeight;

						
							//in case of a douplicated edges at the starting vertex !!
							if((int)nextVertexname == (int)firstVertex)
							{

//								nextVertexname = curR;
//								nextVertexIdx = idx_vertices[&curR];
								doubleWeightedEdgeAtStart = true;



							}
							else
							{
								multigraph[idxReplaceWeightR][idxReplaceWeightC] = 0;
								multigraph[idxReplaceWeightC][idxReplaceWeightR] = 0;
							}


								returnToPrevVertex = false;

							break;
						}


					}
				}
			}

			if(j == multigraph.size() - 1)
			{
				multigraph[idxReplaceWeightR][idxReplaceWeightC] = 0;
				multigraph[idxReplaceWeightC][idxReplaceWeightR] = 0;
				nextVertexIdx = idxLowestVertexWeight;
				nextVertexname = multigraph[0][idxLowestVertexWeight];
			}
		}
		if(nextVertexname != '\0')
		{
			eTour.push_back(&nextVertexname);
		}

		if((int)firstVertex == (int)nextVertexname || nextVertexname == '\0')
		{
			if(doubleWeightedEdgeAtStart)
			{
				doubleWeightedEdgeAtStart = false;
			}
			else
			{
				isStartingVertex = true;
			}
			
			
			//in case of the douplicated edges at start
			if(nextVertexname == '\0')
			{
				std::string lastV = eTour.back();
				int idxLastV = idx_vertices[lastV];
				int idxFirstV = idx_vertices[&firstVertex];

				int weightOfEdge = adj_mx[idxFirstV][idxLastV];
				if(weightOfEdge > 0)
				{
					eTour.push_back(&firstVertex);
				}
				else
				{
					std::cerr << "ERROR WARNING! :  Last vertex has no edge to the start vertex [ " << firstVertex << " ] ! \n";
				}
			}


		}

	}

	std::cout << "Eulerian-Tour: \n";

	for(auto i = 0; i < eTour.size(); i++)
	{
		if(i != eTour.size() - 1)
		{

			std::cout << eTour[i] << " -> ";
		}
		else
		{

			std::cout << eTour[i];
		}
	}
	std::cout << "\n";




	//Eulerian Tour shortcut.
	std::unordered_map<std::string, bool> visisedVertices;
	std::vector<std::string> finalTour;
	for(int i = 0; i < eTour.size();i++)
	{
	    std::string vName = eTour[i];
		
		if(visisedVertices.count(vName) == 0)
		{
			finalTour.push_back(vName);
			visisedVertices[vName] = true;
		}
		if(vName.compare(eTour[1]) && i == eTour.size() - 1)
		{
			finalTour.push_back(vName);
		}

	}

	// Print out Final TSP Tour
	std::cout << "Eulerian-Tour shortcut / Final TSP Tour ! \n";
	int totalTspWeight = 0;
	for(auto i = 0; i < finalTour.size(); i++)
	{
		if(i != finalTour.size() - 1)
		{
			//summ all weight
			std::string v1 = finalTour[i];
			std::string v2 = finalTour[i + 1];
			std::string eName = v1 < v2 ? v1 + v2 : v2 + v1;

			totalTspWeight += edgeMap[eName].weight;

			std::cout << finalTour[i] << " -> ";
		}
		else
		{

			std::cout << finalTour[i] << "\n";
			
		}
	}
	std::cout << "Total weight : " << totalTspWeight << "\n";
	std::cout << "\n";
	std::cout << "----------------[End Eulerian Tour]-----------------\n";
	std::cout << "--------------[ END OF PROGRAMM ]-------------------\n";
	return 0;
}

