#include<iostream>
#include<algorithm>
#include<climits>
#include<vector>
#include<string>
#include<set> 
#include<stack> 
#include<queue> 
#include<map> 
#include<unordered_map>
#include<unordered_set>
#include<cmath>
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

// using namespace std ; 
// #include <bits/stdc++.h>
using namespace std;
#define pb push_back 
#define ll long long 
#define vi vector<int> 
#define vii vector<vi>
#define viii vector<vii>
#define viiii vector<viii>
#define rep(i,a,b) for(int i = a; i<b;i++)
#define print(x) cout <<x<<" "
#define input(x) int x ; cin >> x
#define inputvec(nums, n) rep(j,0,n) cin>>nums[j] 
#define all(x) x.begin(), x.end()
#define pii pair<int,int> 
#define mod 1000000007
#define ff first 
#define ss second
#define fastio ios_base::sync_with_stdio(false); cin.tie(NULL) 
#define nl cout<<endl
#define flag(i) cout <<"here "<<i<<endl 
#define printvec(x) rep(j,0,x.size()){cout <<x[j]<<" "; } nl
#define vec vector

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

int main() {
    // fastio;
    // #ifndef ONLINE_JUDGE
    // freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
    // #endif

    // input(t) ; 
    // while(t--){
    //     solve() ; 
    // } 
    vector<float> v{0.6,0.3,0.1} ; 
    cout <<sample_value(v) <<endl ; 
     

}


