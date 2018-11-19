/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"

using namespace std;

Point<3> convertToLAB(HSLAPixel pixel) {
    Point<3> result(pixel.h/360, pixel.s, pixel.l);
    return result;
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */

    int rows = theSource.getRows();
    int cols = theSource.getColumns();
    MosaicCanvas* ret = new MosaicCanvas(rows, cols);

    vector<Point<3>> points;
    map<Point<3>, int> tile_avg_map;

    for(unsigned i = 0; i < theTiles.size(); i++) {
        HSLAPixel pix = theTiles[i].getAverageColor();
        Point<3> pt = convertToLAB(pix);
        tile_avg_map[pt] = (int)i;
        points.push_back(pt);
    }

    KDTree<3> tree(points);
    TileImage* repIm;

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            repIm = get_match_at_idx(tree, tile_avg_map, theTiles, theSource, i, j);
            ret->setTile(i, j, repIm);
        }
    }

    return ret;
}

TileImage* get_match_at_idx(const KDTree<3>& tree,
                                  map<Point<3>, int> tile_avg_map,
                                  vector<TileImage>& theTiles,
                                  const SourceImage& theSource, int row,
                                  int col)
{
    // Create a tile which accurately represents the source region we'll be
    // using
    HSLAPixel avg = theSource.getRegionColor(row, col);
    Point<3> avgPoint = convertToLAB(avg);
    Point<3> nearestPoint = tree.findNearestNeighbor(avgPoint);

    // Check to ensure the point exists in the map
    map< Point<3>, int >::iterator it = tile_avg_map.find(nearestPoint);
    if (it == tile_avg_map.end())
        cerr << "Didn't find " << avgPoint << " / " << nearestPoint << endl;

    // Find the index
    int index = tile_avg_map[nearestPoint];
    return &theTiles[index];

}
