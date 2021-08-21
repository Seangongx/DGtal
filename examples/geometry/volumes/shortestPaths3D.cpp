/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * @file geometry/volumes/shortestPaths3D.cpp
 * @ingroup Examples
 * @author Jacques-Olivier Lachaud (\c jacques-olivier.lachaud@univ-savoie.fr )
 * Laboratory of Mathematics (CNRS, UMR 5127), University of Savoie, France
 *
 * @date 2021/06/20
 *
 * An example file named shortestPaths3D
 *
 * This file is part of the DGtal library.
 */


/**
   This example shows how to analyze the local geometry of 3D digital
   sets with full convexity over cubical neighborhoods.
   
   @see \ref dgtal_dconvexityapp_sec1
   
   For instance, you may call it to analyse image Al.100.vol at scale 2 as

\verbatim
shortestPaths3D 2 ${DGTAL}/examples/samples/Al.100.vol
\endverbatim

   Results are displayed, then saved in 'geom-cvx.obj'. You may also
   analyse the same shape in multiscale fashion with

\verbatim
shortestPaths3D 2 ${DGTAL}/examples/samples/Al.100.vol
\endverbatim
 
   The result is saved in 'geom-scale-cvx.obj'. You will obtain images
   like below, where green means convex, blue means concave, white is
   planar and red is atypical (see \cite lachaud_dgmm_2021 for details).

<table>
<tr><td>
\image html al100-analysis-1.jpg "Full convexity analysis at scale 1" width=100%
</td><td>
\image html al100-analysis-2.jpg "Full convexity analysis at scale 2" width=100%
</td><td>
\image html al100-analysis-3.jpg "Full convexity analysis at scale 3" width=100%
</td><td>
\image html al100-smooth-analysis-1-5.jpg "Full convexity smooth multiscale analysis (scales 1-5)" width=100%
</td></tr>
</table>

 \example geometry/volumes/shortestPaths3D.cpp
 */


///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <queue>
#include "DGtal/base/Common.h"
#include "DGtal/io/viewers/Viewer3D.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include "DGtal/io/Color.h"
#include "DGtal/io/colormaps/SimpleDistanceColorMap.h"
#include "DGtal/shapes/Shapes.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/helpers/Shortcuts.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/geometry/volumes/TangencyComputer.h"
#include "ConfigExamples.h"

///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace DGtal;
typedef Z3i::Space          Space;
typedef Z3i::KSpace         KSpace;
typedef Z3i::Domain         Domain;
typedef Z3i::SCell          SCell;
typedef Shortcuts< KSpace > SH3;
typedef Space::Point        Point;
typedef Space::RealPoint    RealPoint;
typedef Space::Vector       Vector;

// Called when an user clicks on a surfel.
int reaction( void* viewer, int32_t name, void* data )
{
  int32_t* selected = (int32_t*) data;
  *selected = name;
  std::cout << "Selected surfel=" << *selected << std::endl;
  return 0;
}

int main( int argc, char** argv )
{
  trace.info() << "Usage: " << argv[ 0 ] << " <input.vol> <m> <M> <opt>" << std::endl;
  trace.info() << "\tComputes shortest paths to a source point" << std::endl;
  trace.info() << "\t- input.vol: choose your favorite shape" << std::endl;
  trace.info() << "\t- m [==0], M [==255]: used to threshold input vol image" << std::endl;
  trace.info() << "\t- opt >= sqrt(3): secure shortest paths, 0: fast" << std::endl;
  string inputFilename = examplesPath + "samples/Al.100.vol";
  std::string fn= argc > 1 ? argv[ 1 ]         : inputFilename;
  int         m = argc > 2 ? atoi( argv[ 2 ] ) : 0;
  int         M = argc > 3 ? atoi( argv[ 3 ] ) : 255;
  double    opt = argc > 4 ? atof( argv[ 4 ] ) : sqrt(3.0);
  QApplication application(argc,argv);
  Viewer3D<> viewer;
  viewer.setWindowTitle("shortestPaths3D");
  viewer.show();  

  // Set up shortcuts parameters.
  auto   params  = SH3::defaultParameters();
  params( "thresholdMin", m )( "thresholdMax", M );
  params( "surfaceComponents" , "All" );
  
  // Domain creation from two bounding points.
  trace.info() << "Building set or importing vol ... ";
  KSpace K;
  auto bimage = SH3::makeBinaryImage( fn, params );
  K = SH3::getKSpace( bimage );
  Point lo = K.lowerBound();
  Point hi = K.upperBound();
  Domain domain = Domain( lo, hi );
  trace.info() << "  [Done]" << std::endl;
  
  // Compute surface
  auto surface = SH3::makeDigitalSurface( bimage, K, params );

  
  // Compute interior boundary points
  // They are less immediate interior points than surfels.
  std::vector< Point >   points;
  std::map< SCell, int > surfel2idx;
  std::map< Point, int > point2idx;
  int idx = 0;
  for ( auto s : (*surface) )
    {
      // get inside point on the border of the shape.
      Dimension k = K.sOrthDir( s );
      auto voxel  = K.sIncident( s, k, K.sDirect( s, k ) );
      Point p     = K.sCoords( voxel );
      auto it     = point2idx.find( p );
      if ( it == point2idx.end() )
        {
          points.push_back( p );
          surfel2idx[ s ] = idx;
          point2idx [ p ] = idx++;
        }
      else
        surfel2idx[ s ] = it->second;
    } 
  trace.info() << "Shape has " << points.size() << " interior boundary points"
               << std::endl;

  // Select a starting point.
  typedef Viewer3D<> MViewer3D;
  int32_t selected_surfels[ 2 ] = { 0, 0 };
  auto surfels = SH3::getSurfelRange ( surface );
  for ( int i = 0;  i < 2; i++ )
    {
      MViewer3D viewerCore( K );
      viewerCore.show();
      Color colSurfel( 200, 200, 255, 255 );
      Color colStart( 255, 0, 0, 255 );
      int32_t name = 0;
      viewerCore << SetMode3D( surfels[ 0 ].className(), "Basic");
      viewerCore.setFillColor( colSurfel );
      for ( auto && s : surfels ) viewerCore << SetName3D( name++ ) << s;
      viewerCore << SetSelectCallback3D( reaction, &selected_surfels[ i ],
                                         0, surfels.size() - 1 );
      viewerCore << MViewer3D::updateDisplay;
      application.exec();
    }
  
  // Get selected surfel/point
  const auto s0 = surfels[ selected_surfels[ 0 ] ];
  Dimension  k0 = K.sOrthDir( s0 );
  auto   voxel0 = K.sIncident( s0, k0, K.sDirect( s0, k0 ) );
  Point      p0 = K.sCoords( voxel0 );
  auto   start0 = point2idx[ p0 ];
  std::cout << "Start0 index is " << start0 << std::endl;
  const auto s1 = surfels[ selected_surfels[ 1 ] ];
  Dimension  k1 = K.sOrthDir( s1 );
  auto   voxel1 = K.sIncident( s1, k1, K.sDirect( s1, k1 ) );
  Point      p1 = K.sCoords( voxel1 );
  auto   start1 = point2idx[ p1 ];
  std::cout << "Start1 index is " << start1 << std::endl;

  // Use Tangency to compute shortest paths
  typedef TangencyComputer< KSpace >::Index Index;
  TangencyComputer< KSpace > TC( K );
  TC.init( points.cbegin(), points.cend() );
  auto SP = TC.makeShortestPaths( opt );
  SP.init( start0 ); //< set source
  double last_distance = 0.0;
  while ( ! SP.finished() )
    {
      last_distance = std::get<2>( SP.current() );
      SP.expand();
    }
  std::cout << "Max distance is " << last_distance << std::endl;

  {
    const int nb_repetitions = 10;
    const double      period = last_distance / nb_repetitions;
    SimpleDistanceColorMap< double > cmap( 0.0, period, false );
    MViewer3D viewerCore;
    viewerCore.show();
    Color colSurfel( 200, 200, 255, 128 );
    Color colStart( 255, 0, 0, 128 );

    viewerCore.setUseGLPointForBalls(true);
    for ( auto i = 0; i < points.size(); ++i )
      {
        const double d_s = SP.distance( i );
        Color c_s        = cmap( fmod( d_s, period ) );
        viewerCore.setFillColor( c_s );
        viewerCore.addBall( RealPoint( points[ i ][ 0 ],
                                       points[ i ][ 1 ],
                                       points[ i ][ 2 ] ), 12.0 );
      }
    // auto surfels = SH3::getSurfelRange ( surface );
    // viewerCore << SetMode3D( surfels[ 0 ].className(), "Basic");
    // for ( auto && s : surfels )
    //   {
    //     const double d_s = SP.distance( surfel2idx[ s ] );
    //     Color c_s        = cmap( fmod( d_s, period ) );
    //     viewerCore.setFillColor( c_s );
    //     viewerCore << s;
    //   }
    // viewerCore.setFillColor( colStart );
    // viewerCore.setLineColor( Color::Green );
    // viewerCore << s;
    // for ( Index i = 0; i < SP.size(); i++ ) {
    //   Point p1 = SP.point( i );
    //   Point p2 = SP.point( SP.ancestor( i ) );
    //   viewerCore.addLine( p1, p2, 1.0 );
    // }
    viewerCore << MViewer3D::updateDisplay;
    application.exec();
  }

  // Extracts a shortest path between two points.
  auto SP0 = TC.makeShortestPaths( opt );
  auto SP1 = TC.makeShortestPaths( opt );
  SP0.init( start0 ); 
  SP1.init( start1 ); 
  last_distance = 0.0;
  std::vector< Index > Q;
  while ( ! SP0.finished() && ! SP1.finished() )
    {
      auto n0 = SP0.current();
      auto n1 = SP1.current();
      auto p0 = std::get<0>( n0 );
      auto p1 = std::get<0>( n1 );
      SP0.expand();
      SP1.expand();
      if ( SP0.isVisited( p1 ) )
        {
          auto c0 = SP0.pathToSource( p1 );
          auto c1 = SP1.pathToSource( p1 );
          std::copy(c0.rbegin(), c0.rend(), std::back_inserter(Q));
          Q.pop_back();
          std::copy(c1.begin(), c1.end(), std::back_inserter(Q)); 
          break;
        }
      last_distance = std::get<2>( n0 ) + std::get<2>( n1 );
      std::cout << p0 << " " << p1 << " last_d=" << last_distance << std::endl;
    }
  
  std::cout << "Max distance is " << last_distance << std::endl;

  {
    const int nb_repetitions = 10;
    const double      period = last_distance / nb_repetitions;
    SimpleDistanceColorMap< double > cmap( 0.0, period, false );
    MViewer3D viewerCore;
    viewerCore.show();
    Color colSurfel( 200, 200, 255, 128 );
    Color colStart( 255, 0, 0, 128 );
    viewerCore.setUseGLPointForBalls(true);
    for ( auto i = 0; i < points.size(); ++i )
      {
        const double d_s0 = SP0.isVisited( i ) ? SP0.distance( i ) : SP0.infinity();
        const double d_s1 = SP1.isVisited( i ) ? SP1.distance( i ) : SP1.infinity();
        const double d_s  = std::min( d_s0, d_s1 );
        Color c_s        = ( d_s != SP0.infinity() )
          ? cmap( fmod( d_s, period ) )
          : Color::Black;
        viewerCore.setFillColor( c_s );
        viewerCore.addBall( RealPoint( points[ i ][ 0 ],
                                       points[ i ][ 1 ],
                                       points[ i ][ 2 ] ), 12 );
      }
    viewerCore.setLineColor( Color::Green );
    bool first = true;
    Index prev = 0;
    for ( auto p : Q )
      {
        if ( first )
          {
            first = false;
            prev  = p;
          }
        else
          {
            Point p1 = SP0.point( prev );
            Point p2 = SP0.point( p );
            viewerCore.addLine( p1, p2, 18.0 );
            prev = p;
          }
      }
    viewerCore << MViewer3D::updateDisplay;
    application.exec();
  }
  
  return 0;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

