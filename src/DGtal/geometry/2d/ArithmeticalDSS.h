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
//LICENSE-END
#pragma once

/**
 * @file ArithmeticalDSS.h @author Tristan Roussillon (\c
 * tristan.roussillon@liris.cnrs.fr ) Laboratoire d'InfoRmatique en
 * Image et Systèmes d'information - LIRIS (CNRS, UMR 5205), CNRS,
 * France
 *
 * @date 2010/07/01
 *
 * Header file for module ArithmeticalDSS.cpp
 *
 * This file is part of the DGtal library.
 */

#if defined(ArithmeticalDSS_RECURSES)
#error Recursive header files inclusion detected in ArithmeticalDSS.h
#else // defined(ArithmeticalDSS_RECURSES)
/** Prevents recursive inclusion of headers. */
#define ArithmeticalDSS_RECURSES

#if !defined ArithmeticalDSS_h
/** Prevents repeated inclusion of headers. */
#define ArithmeticalDSS_h

//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include "DGtal/base/Exceptions.h"
#include "DGtal/base/Common.h"
#include "DGtal/kernel/PointVector.h"
#include "DGtal/kernel/CInteger.h"
//////////////////////////////////////////////////////////////////////////////


namespace DGtal
{


/////////////////////////////////////////////////////////////////////////////
// class ArithmeticalDSS
/**
 * Description of class 'ArithmeticalDSS' <p>
 * \brief Aim:
 * Recognition of a digital straight segment (DSS)
 * defined as the sequence of connected points (x,y)
 * such that mu <= ax - by < mu + omega.  
 * (see Debled and Reveilles (1995)).
 */
template <typename TDSS>
class ArithmeticalDSS
{


    // ----------------------- Types ------------------------------
public:

		//2D point and 2D vector
	  //BOOST_CONCEPT_ASSERT(( CInteger<TInteger> ) );
		//does not compile
		typedef typename TDSS::Integer Integer;
		typedef DGtal::PointVector<2,Integer> Point;
		typedef DGtal::PointVector<2,Integer> Vector;
		
		typedef DGtal::PointVector<2,double> PointD;

    // ----------------------- Standard services ------------------------------
public:

    /**
     * Constructor.
     */
    ArithmeticalDSS();

    /**
     * Constructor.
		 * @param aFirstPoint and aSecondPoint, 
		 * two connected points for initialisation
     */
    ArithmeticalDSS(const Point& aFirstPoint, const Point& aSecondPoint);


    /**
     * Copy constructor.
     * @param other the object to clone.
     * Forbidden by default.
     */
    ArithmeticalDSS ( const ArithmeticalDSS & other );

    /**
     * Assignment.
     * @param other the object to copy.
     * @return a reference on 'this'.
     * Forbidden by default.
     */
    ArithmeticalDSS & operator= ( const ArithmeticalDSS & other );

    /**
     * Equality operator.
     * @param other the object to compare with.
     * @return 'true' either if the points perfectly match
	 	 * or if the first points match to the last ones
     * (same DSS scanned in the conversed way) 
     * and 'false' otherwise
     */
		bool operator==( const ArithmeticalDSS & other ) const;

    /**
     * Difference operator.
     * @param other the object to compare with.
     * @return 'false' if equal
     * 'true' otherwise
     */
		bool operator!=( const ArithmeticalDSS & other ) const;

    /**
     * Destructor.
     */
    ~ArithmeticalDSS(){};

    // ----------------------- Interface --------------------------------------
public:
     


    /**
		 * Tests whether the union between a point 
     * (adding to the front of the DSS 
     * with respect to the scan orientaion) 
		 * and a DSS is a DSS. 
     * Computes the parameters of the new DSS 
     * with the adding point if true.
     * @param aPoint the new pixel (connected to the DSS) 
     * @return 'true' if the union is a DSS, 'false' otherwise.
     */
    bool addFront(const Point & aPoint);

    /**
		 * Removes the first point of a DSS
     * (located at the back with respect to 
     * the scan orientaion)
	   * if the DSS has more than two points
     * @return 'true' if the first point is removed, 'false' otherwise.
     */
    bool removeBack();

    /**
		 * Computes the sequence of (connected) points
		 * belonging to the DSL(a,b,mu,omega)
     * between the first and last point of the DSS
		 * Nb: in O(omega)
     * @return the computed sequence of points.
     */
    std::vector<Point> recover() const;


    /**
		 * Computes the remainder of a point
     * (that does not necessarily belong to the DSS)
     * @param aPoint the point whose remainder is returned 
     * @return the remainder.
     */
    Integer getRemainder( const Point& aPoint ) const;

    /**
		 * Checks whether a point is in the DSL
     * of parameters (myA,myB,myMu,myOmega)
     * @return 'true' if yes, 'false' otherwise
     */
    bool isInDSL( const Point& aPoint ) const;

    /**
		 * Checks whether a point belongs to the DSS or not
     * @return 'true' if yes, 'false' otherwise
     */
    bool isInDSS( const Point& aPoint ) const;

    // ------------------------- Accessors ------------------------------
    /**
		 * myA accessor
     * @return an Integer of value myA.
     */
    Integer getA() const;
    /**
		 * myB accessor
     * @return an Integer of value myB.
     */
    Integer getB() const;
    /**
		 * myMu accessor
     * @return an Integer of value myMu.
     */
    Integer getMu() const;
    /**
		 * myOmega accessor
     * @return an Integer of value myOmega.
     */
    Integer getOmega() const;
    /**
		 * Accessor to the first upper leaning point
     * @return first upper leaning point.
     */
    Point getUf() const;
    /**
		 * Accessor to the last upper leaning point
     * @return last upper leaning point.
     */
    Point getUl() const;
    /**
		 * Accessor to the first lower leaning point
     * @return first lower leaning point.
     */
    Point getLf() const;
    /**
		 * Accessor to the last lower leaning point
     * @return last lower leaning point.
     */
    Point getLl() const;
    /**
		 * Accessor to the first point of the DSS
     * @return first point.
     */
    Point getF() const;
    /**
		 * Accessor to the last point of the DSS
     * @return last point.
     */
    Point getL() const;

    /**
     * Checks the validity/consistency of the object.
     * @return 'true' if the object is valid, 'false' otherwise.
     */
    bool isValid() const;

// ------------------ Display ------------------------------------------

	public:
    /**
   * Projects the point [m] onto the average straight line (ie (mu+nu)/2).
   * @param m any point expressed in the local reference frame (may not be part of the segment).
   * @return the projected point.
   */
    PointD project( const Point & m ) const;

    /**
     * Projects the point [m] onto the straight line whose points have
     * remainder [r].
     *
     * @param m any point expressed in the local reference frame (may not
     * be part of the segment).
     *
     * @param r the remainder (may not be an integer).
     * @return the projected point.
     */
    PointD project( const Point & m, float r ) const;
    
    /**
     * Projects the point [m] onto the straight line going through point [p].
     *
     * @param m any point expressed in the local reference frame (may not
     * be part of the segment).
     *
     * @param p any point expressed in the local reference frame (may not
     * be part of the segment).
     *
     * @return the projected point.
     */
    PointD project( const Point & m, const Point & p ) const;
  

    /**
     * Defined as: norm( project(cp_n) - project(c_n) )
     * @return the projected length of the segment.
     * @see projectRegularly
     */
    double projectedSegmentLength() const;
    

    /**
     * Writes/Displays the object on an output stream.
     * @param out the output stream where the object is written.
     */
    void selfDisplay ( std::ostream & out ) ;


    /**
     * Draw the bounding box of the DSS on a LiBoard board
     * @param board the output board where the object is drawn.
     * @tparam Functor a Functor to specialize the Board style
     */
    template<typename Functor>
      void BoundingBoxDraw( LibBoard::Board & board ) const;
    
    /**
     * Draw the retrieved digital points of the DSS linked into a 
     * polygonal line on a LiBoard board
     * @param board the output board where the object is drawn.
     * @tparam Functor a Functor to specialize the Board style
     */
    template<typename Functor>
      void DigitalPointsDraw( LibBoard::Board & board ) const;


    /**
     * Draw the object on a LiBoard board
     * @param board the output board where the object is drawn.
     * @tparam Functor a Functor to specialize the Board style
     */
    void selfDraw( LibBoard::Board & board ) const
      {
				BoundingBoxDraw<BoundingBoxStyle>(board);
				DigitalPointsDraw<DigitalPointsStyle>(board);
      }

private:

   /** 
     * Default Style Functor for bounding box drawing
     * 
     * @param aBoard 
     */

    struct BoundingBoxStyle
    {
      BoundingBoxStyle(LibBoard::Board & aBoard) 
      {
				aBoard.setFillColor(LibBoard::Color::None);
				aBoard.setPenColor(LibBoard::Color::Red);
				aBoard.setLineWidth(1);
      }
    };

   /** 
     * Default Style Functor for digital points drawing
     * 
     * @param aBoard 
     */

    struct DigitalPointsStyle
    {
      DigitalPointsStyle(LibBoard::Board & aBoard) 
      {
				aBoard.setFillColor(LibBoard::Color::None);
				aBoard.setPenColor(LibBoard::Color::Black);
				aBoard.setLineWidth(2);
      }
    };
    // ------------------------- Protected Datas ------------------------------
protected:

	//parameters of the DSS
	Integer myA, myB, myMu, myOmega;

	//leaning points
	Point myUf, myUl, myLf, myLl;

	//first and last point
	Point myF, myL;

    // ------------------------- Private Datas --------------------------------
	
private:

    // ------------------------- Hidden services ------------------------------
private:


    /**
		 * Returns the 2D vector 
		 * starting at a point of remainder 0
		 * and pointing at a point of remainder omega
     * @return the 2D vector.
     */
    Vector vectorFrom0ToOmega() const;

    /**
		 * Returns the point that follows a given 
     * point of a DSL
		 * @param a, b, mu, omega, the parameters 
     * of the DSL and aPoint, a given point of 
     * the DSL. 
     * @return the next point.
     */
    Point next(const Point& aPoint) const;




}; // end of class ArithmeticalDSS


/////////////////////////////////////////////////////////////////////////////
// class StandardBase
/**
 * Description of class 'StandardBase' <p>
 * \brief Aim:
 * Provide static methods for the 
 * recognition of a standard DSS (4-connected)
 * such that omega = |a|+|b| 
 */
template <typename TInteger>
class StandardBase
{

    // ----------------------- Types ------------------------------
public:

		//2D point and 2D vector
	  BOOST_CONCEPT_ASSERT(( CInteger<TInteger> ) );
		typedef TInteger Integer;
		typedef DGtal::PointVector<2,Integer> Point;
		typedef DGtal::PointVector<2,Integer> Vector;


    // ----------------------- Methods ----------------------------
    /**
		 * Computes the L1 norm of the two components
     * of a 2D vector.
		 * @param x and y, two values. 
     * @return the L1 norm of a 2D vector.
     */
		static Integer norm(const Integer& x, const Integer& y);

    /**
		 * Checks whether the DSS has less or more
     * than two displacement vectors (steps)
     * between consecutive points
		 * @param a, b, the slope of the DSS and 
     * aStep, the last displacement vector. 
     * @return 'true' if yes, 'false' otherwise.
     */
    static bool hasMoreThanTwoSteps(const Integer& a, 
														const Integer& b,
														const Vector& aStep);

    /**
		 * Returns the 2D vector 
		 * corresponding to '0'
     * in the Freeman representation
     * in the first octant
		 * @param a, b, the slope of the DSS
     * @return the 2D vector.
     */
    static Vector step0(const Integer& a, const Integer& b);

    /**
		 * Returns the 2D vector 
		 * corresponding to '1' 
     * in the Freeman representation
     * in the first octant
		 * @param a, b, the slope of the DSS
     * @return the 2D vector.
     */
    static Vector step1(const Integer& a, const Integer& b);


};

/////////////////////////////////////////////////////////////////////////////
// class NaiveBase
/**
 * Description of class 'NaiveBase' <p>
 * \brief Aim:
 * Provide elementary services for the 
 * recognition of a naive DSS (8-connected)
 * such that omega = max(|a|,|b|) 
 */
template <typename TInteger>
class NaiveBase
{

    // ----------------------- Types ------------------------------
public:

		//2D point and 2D vector
	  BOOST_CONCEPT_ASSERT(( CInteger<TInteger> ) );
		typedef TInteger Integer;
		typedef DGtal::PointVector<2,Integer> Point;
		typedef DGtal::PointVector<2,Integer> Vector;


    // ----------------------- Methods ----------------------------
    /**
		 * Computes the Linf norm of the two components
     * of a 2D vector.
		 * @param x and y, two values. 
     * @return the Linf norm of a 2D vector.
     */
		static Integer norm(const Integer& x, const Integer& y);

    /**
		 * Checks whether the DSS has less or more
     * than two displacement vectors (steps)
     * between consecutive points
		 * @param a, b, the slope of the DSS and 
     * aStep, the last displacement vector. 
     * @return 'true' if yes, 'false' otherwise.
     */
    static bool hasMoreThanTwoSteps(const Integer& a, 
														const Integer& b,
														const Vector& aStep);

    /**
		 * Returns the 2D vector 
		 * corresponding to '0'
     * in the Freeman representation
     * in the first octant
		 * @param a, b, the slope of the DSS
     * @return the 2D vector.
     */
    static Vector step0(const Integer& a, const Integer& b);

    /**
		 * Returns the 2D vector 
		 * corresponding to '1' 
     * in the Freeman representation
     * in the first octant
		 * @param a, b, the slope of the DSS
     * @return the 2D vector.
     */
    static Vector step1(const Integer& a, const Integer& b);



};



/**
 * Overloads 'operator<<' for displaying objects of class 'ArithmeticalDSS'.
 * @param out the output stream where the object is written.
 * @param object the object of class 'ArithmeticalDSS' to write.
 * @return the output stream after the writing.
 */
template <typename TDSS>
std::ostream&
operator<< ( std::ostream & out,  ArithmeticalDSS<TDSS> & object )
  {
      object.selfDisplay( out);
      return out;
    }


} // namespace DGtal


///////////////////////////////////////////////////////////////////////////////
// Includes inline functions/methods if necessary.
#if defined(INLINE)
#include "DGtal/geometry/2d/ArithmeticalDSS.ih"
#endif

//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#endif // !defined ArithmeticalDSS_h

#undef ArithmeticalDSS_RECURSES
#endif // else defined(ArithmeticalDSS_RECURSES)