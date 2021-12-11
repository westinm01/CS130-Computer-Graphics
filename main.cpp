#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <stdexcept>
using namespace std;

/*
  Vertex order:
  2---3
  |   |
  0---1

  Edge order:
  * 2 *
  1   3
  * 0 *

 */

// For each edge of the square, the indices of the vertices at its endpoints.
int edge_to_vertex[4][2] = {{0,1}, {0,2}, {2,3}, {1,3}};

static const int max_segments = 2;

// case_table[case_number][i][a] is an edge index (see above) indicating which
// edges of the square are at the endpoints of the segment.
// i identifies the segment (0, 1)
// a identifies the endpoint (0, 1).
// Fill in the table below.  Unused segments should be left as {0,0}, which does
// not code for a valid segment.  When using the table, test for {0,0} to see if
// there are anymore segments left to emit.
// TODO: Fill in this table.
int case_table[16][max_segments][2] =
{
    {{0,0},{0,0}},
    {{0,1},{0,0}},
    {{0,3},{0,0}},
    {{1,3},{0,0}},
    {{1,2},{0,0}},
    {{0,2},{0,0}},
    {{0,3},{1,2}},
    {{2,3},{0,0}},
    {{2,3},{0,0}},
    {{0,1},{2,3}},
    {{0,2},{0,0}},
    {{1,2},{0,0}},
    {{1,3},{0,0}},
    {{0,3},{0,0}},
    {{0,1},{0,0}},
    {{0,0},{0,0}}
};

struct grid_array
{
    int m=0, n=0;
    vector<float> data;

    void resize(int i, int j)
    {
        m=i;
        n=j;
        data.resize(m*n);
    }

    const float& operator()(int i, int j) const
    {
        assert(i<m);
        assert(j<n);
        return data[i*n+j];
    }

    float& operator()(int i, int j)
    {
        assert(i<m);
        assert(j<n);
        return data[i*n+j];
    }
};

ofstream out;

void start_output(int width, int height)
{
    out.open("output.eps");
    out << "%!PS-Adobe-3.0 EPSF-3.0\n";
    out << "%%BoundingBox: 0 0 " << (width-1)*100 << " " << (height-1)*100 << "\n";
    out << ".02 setlinewidth\n";
}

void finish_output()
{
    out << "showpage\n";
    out.close();
}

void output_segment(float x0, float y0, float x1, float y1)
{
    out << x0*100 << ' ' << y0*100 << " moveto " << x1*100 << ' ' << y1*100 << " lineto stroke\n";
}

void set_color(float r, float g, float b)
{
    out << r << ' ' << g << ' ' << b << " setrgbcolor\n";
}

void render_grid(grid_array& phi)
{
    set_color(0, 0, 0);
    for(int y = 0; y < phi.n; y++)
        output_segment(0,y,phi.m-1,y);
    for(int x = 0; x < phi.m; x++)
        output_segment(x,0,x,phi.n-1);
    for(int x = 0; x < phi.m; x++)
        for(int y = 0; y < phi.n; y++)
        {
            if(phi(x,y)<0) set_color(0, 1, 0);
            else set_color(1, 0, 0);
            out << x*100 << ' ' << y*100 << " 10 0 360 arc fill\n";
        }
    set_color(0, 0, 1);
}

void read(ifstream &input, grid_array& phi)
{
    string line;
    int height=0, width=0;
    int j=0;
    while(getline(input, line))
    {
        if(sscanf(line.c_str(),"# rows: %d",&height)==1 ||
            sscanf(line.c_str(),"# columns: %d",&width)==1)
        {
            if(height>0 && width>0) phi.resize(width, height);
            continue;
        }
        if(line[0]=='#' || line.empty()) continue;
        stringstream iss;
        iss.str(line);
        for(int i=0;i<phi.m;i++)
            iss>>phi(i,j);
        if(iss) j++;
    }
}

int compute_case(float cell_data[4])
{
    int c=0;
    if(cell_data[0]<0){
        c+=1;
    }
    if(cell_data[1]<0){
        c+=2;
    }
    if(cell_data[2]<0){
        c+=4;
    }
    if(cell_data[3]<0){
        c+=8;
    }
    return c;
}

void marching_squares(const grid_array& phi, int i, int j)
{
    float cell_data[4] = {phi(i,j), phi(i+1,j), phi(i,j+1), phi(i+1,j+1)};

    int case_number = compute_case(cell_data);
    auto tab=case_table[case_number];
    (void)tab;
    for(int t=0;t<2;t++){
        if(tab[t][0]&& tab[t][1]==0){
        }
        else{
            float x[2];
            float y[2];
            for(int p =0;p<2;p++){
                switch(tab[t][p]){
                    case 0:
                        x[p]=i+.5;
                        y[p]=j;
                        break;
                    case 1:
                        x[p]=i;
                        y[p]=j+.5;
                        break;
                    case 2:
                        x[p]=i+.5;
                        y[p]=j+1;
                        break;
                    case 3:
                        x[p]=i+1;
                        y[p]=j+.5;
                        break;
                    default:
                    break;
                }
            }
            output_segment(x[0],y[0],x[1],y[1]);
        }
    }
    // TODO: You will need to do a few things here.  Loop over segments.  If the
    // segment is a valid segment, calculate the positions of the endpoints of
    // the segment using the data from the table.  You can also use
    // edge_to_vertex.  Output the segment with output_segment.  Note that the
    // location of the node with index (i,j) is just (i,j).
}

void marching_squares(const grid_array& phi)
{
    for(int i=0; i<phi.m-1; i++)
        for(int j=0; j<phi.n-1; j++)
            marching_squares(phi, i, j);
}

int main(int argc, char *argv[])
{
    assert(argc==2);

    // read input
    ifstream input(argv[1]);
    if(!input.is_open())
        throw runtime_error(string("can't open file ") + argv[1]);

    grid_array phi;
    read(input, phi);

    start_output(phi.m, phi.n);
    render_grid(phi);

    marching_squares(phi);

    finish_output();

    return 0;
}
