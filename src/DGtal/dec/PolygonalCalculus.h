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

#pragma once

/**
 * @file
 * @author David Coeurjolly (\c david.coeurjolly@liris.cnrs.fr )
 * Laboratoire d'InfoRmatique en Image et Systemes d'information - LIRIS (CNRS, UMR 5205), CNRS, France
 *
 * @date 2021/09/02
 *
 * Header file for module PolygonalCalculus.cpp
 *
 * This file is part of the DGtal library.
 */
//////////////////////////////////////////////////////////////////////////////
// Inclusions
#include <iostream>
#include <functional>
#include <vector>
#include "DGtal/base/ConstAlias.h"
#include "DGtal/base/Common.h"
#include "DGtal/math/linalg/EigenSupport.h"
//////////////////////////////////////////////////////////////////////////////

namespace DGtal
{

/////////////////////////////////////////////////////////////////////////////
// template class PolygonalCalculus
/**
 * Description of template class 'PolygonalCalculus' <p>
 * \brief Implements differential operators on polygonal surfaces from
 * @cite degoes2020discrete
 *
 * See @ref modulePolygonalCalculus for details.
 *  E.g.
 *
 */
template <typename TSurfaceMesh>
class PolygonalCalculus
{
  // ----------------------- Standard services ------------------------------
public:
  
  ///Type of SurfaceMesh
  typedef TSurfaceMesh SurfaceMesh;
  
  ///Vertex type
  typedef typename TSurfaceMesh::Vertex Vertex;
  
  ///Face type
  typedef typename TSurfaceMesh::Face Face;
  
  ///Position type
  typedef typename TSurfaceMesh::RealPoint RealPoint;

  ///Real vector type
  typedef typename TSurfaceMesh::RealVector RealVector;
  
  ///Linear Algebra Backend from Eigen
  typedef EigenLinearAlgebraBackend LinAlg;
  ///Type of Vector
    typedef LinAlg::DenseVector Vector;
  ///Type of dense matrix
  typedef LinAlg::DenseMatrix DenseMatrix;
  ///Type of sparse matrix
  typedef LinAlg::SparseMatrix SparseMatrix;
  ///Type of sparse matrix triplet
  typedef LinAlg::Triplet Triplet;

  ///Type of a sparse matrix solver
  typedef LinAlg::SolverSimplicialLDLT Solver;

  /// Create a Polygonal DEC structure from a surface mesh (@a surf)
  /// using an default identity embedder.
  /// @param surf an instance of SurfaceMesh
  PolygonalCalculus(const ConstAlias<TSurfaceMesh> surf): mySurfaceMesh(&surf)
  {
    myEmbedder =[&](Face f,Vertex v){ return mySurfaceMesh->position(v);};
    init();
  };
  
  /// Create a Polygonal DEC structure from a surface mesh (@a surf)
  /// and an embedder for the vertex position: function with two parameters, a face and a vertex
  /// which outputs the embedding in R^3 of the vertex w.r.t. to the face.
  /// @param surf an instance of SurfaceMesh
  /// @param embedder an embedder
  PolygonalCalculus(const ConstAlias<TSurfaceMesh> surf,
               const std::function<RealPoint(Face,Vertex)> &embedder):
  mySurfaceMesh(&surf), myEmbedder(embedder)
  {
    init();
  };
  
  /**
   * Default constructor.
   */
  PolygonalCalculus() = delete;
  
  /**
   * Destructor.
   */
  ~PolygonalCalculus() = default;
  
  /**
   * Copy constructor.
   * @param other the object to clone.
   */
  PolygonalCalculus ( const PolygonalCalculus & other ) = delete;
  
  /**
   * Move constructor.
   * @param other the object to move.
   */
  PolygonalCalculus ( PolygonalCalculus && other ) = delete;
  
  /**
   * Copy assignment operator.
   * @param other the object to copy.
   * @return a reference on 'this'.
   */
  PolygonalCalculus & operator= ( const PolygonalCalculus & other ) = delete;
  
  /**
   * Move assignment operator.
   * @param other the object to move.
   * @return a reference on 'this'.
   */
  PolygonalCalculus & operator= ( PolygonalCalculus && other ) = delete;
  
  // ----------------------- Interface --------------------------------------
  
  
  /// Update the embedding function.
  /// @param externalFunctor a new embedding functor (Face,Vertex)->RealPoint.
  void setEmbedder(const std::function<PolygonalCalculus<TSurfaceMesh>::RealPoint(PolygonalCalculus<TSurfaceMesh>::Face,
                                                                                  PolygonalCalculus<TSurfaceMesh>::Vertex)> &externalFunctor)
  {
    myEmbedder = externalFunctor;
  }
  
  // ----------------------- Per face operators --------------------------------------
  
  /// Return the vertex position matrix degree x 3 of the face.
  /// @param f a face
  /// @return the n_f x 3 position matrix
  DenseMatrix X(const Face f) const;
  

  /// Derivative operator (d_0) of a face.
  /// @param f the face
  /// @return a degree x degree matrix
  DenseMatrix D(const Face f) const;
  
  /// Edge vector operator per face.
  /// @param f the face
  /// @return degree x 3 matrix
  DenseMatrix E(const Face f) const
  {
    return D(f)*X(f);
  }
  
  /// Average operator to average, per edge, its vertex values.
  /// @param f the face
  /// @return a degree x degree matrix
  DenseMatrix A(const Face f) const;
  
  
  /// Polygonal (corrected) vector area.
  /// @param f the face
  /// @return a vector
  Vector vectorArea(const Face f) const;
  
  /// Area of a face from the vector area.
  /// @param f the face
  /// @return the corrected area of the face
  double faceArea(const Face f) const
  {
    return vectorArea(f).norm();
  }
  
  /// Corrected normal vector of a face.
  /// @param f the face
  /// @return a vector (Eigen vector)
  Vector faceNormal(const Face f) const
  {
    Vector v = vectorArea(f);
    v.normalize();
    return v;
  }
  
  /// Corrected normal vector of a face.
  /// @param f the face
  /// @return a vector (DGtal RealVector/RealPoint)
  RealVector faceNormalAsDGtalVector(const Face f) const
  {
    Vector v = faceNormal(f);
    return {v(0),v(1),v(2)};
  }
  
  /// co-Gradient operator of the face
  /// @param f the face
  /// @return a 3 x degree matrix
  DenseMatrix coGradient(const Face f) const
  {
    return  E(f).transpose() * A(f);
  }
  
  ///Return [n] as the 3x3 operator such that [n]q = n x q
  ///@param n a vector
  DenseMatrix bracket(const Vector &n) const
  {
    Eigen::Matrix3d brack;
    brack << 0.0 , -n(2), n(1),
             n(2), 0.0 , -n(0),
            -n(1) , n(0),0.0 ;
    return brack;
  }
  
  /// Gradient operator of the face.
  /// @param f the face
  /// @return 3 x degree matrix
  DenseMatrix gradient(const Face f) const
  {
    return -1.0/faceArea(f) * bracket( faceNormal(f) ) * coGradient(f);
  }
  
  /// Flat operator for the face.
  /// @param f the face
  /// @return a degree x 3 matrix
  DenseMatrix  flat(const Face f) const
  {
    auto n = faceNormal(f);
    return E(f)*( DenseMatrix::Identity(3,3) - n*n.transpose());
  }
  
  /// Edge mid-point operator of the face.
  /// @param f the face
  /// @return a degree x 3 matrix
  DenseMatrix B(const Face f) const
  {
    return A(f) * X(f);
  }
  
  /// @returns the centroid of the face
  /// @param f the face
  Vector centroid(const Face f) const
  {
    auto nf = myFaceDegree[f];
    return 1.0/(double)nf * X(f).transpose() * Vector::Ones(nf);
  }
  
  /// @returns the centroid of the face as a DGtal RealPoint
  /// @param f the face
  RealPoint centroidAsDGtalPoint(const Face f) const
  {
    Vector c = centroid(f);
    return {c(0),c(1),c(2)};
  }
  
  /// Sharp operator for the face.
  /// @param f the face
  /// @return a 3 x degree matrix
  DenseMatrix sharp(const Face f) const
  {
    auto nf = myFaceDegree[f];
    return 1.0/faceArea(f) * bracket(faceNormal(f)) * ( B(f).transpose() - centroid(f)* Vector::Ones(nf).transpose() );
  }
  
  /// Projection operator for the face.
  /// @param f the face
  /// @return a degree x degree matrix
  DenseMatrix P(const Face f) const
  {
    auto nf = myFaceDegree[f];
    return DenseMatrix::Identity(nf,nf) - flat(f)*sharp(f);
  }
  
  /// Inner product on 1-forms associated with the face
  /// @param f the face
  /// @param lambda the regularization parameter
  /// @return a degree x degree matrix
  DenseMatrix M(const Face f, const double lambda=1.0) const
  {
    auto Uf=sharp(f);
    auto Pf=P(f);
    return faceArea(f) * Uf.transpose()*Uf + lambda * Pf.transpose()*Pf;
  }
  
  /// Divergence operator of a one-form.
  /// @param f the face
  /// @param lambda the regularization parameter
  /// @return a degree x degree matrix
  DenseMatrix divergence(const Face f, const double lambda=1.0) const
  {
    return D(f).transpose() * M(f);
  }
  
  /// Curl operator of a one-form (identity matrix).
  /// @param f the face
  /// @return a degree x degree matrix
  DenseMatrix curl(const Face f) const
  {
    return DenseMatrix::Identity(myFaceDegree[f],myFaceDegree[f]);
  }
  
  
  /// (weak) Laplace-Beltrami operator for the face.
  /// @param f the face
  /// @param lambda the regularization parameter
  /// @return a degree x degree matrix
  DenseMatrix LaplaceBeltrami(const Face f, const double lambda=1.0) const
  {
    auto Df = D(f);
    return Df.transpose() * M(f,lambda) * Df;
  }

  // ----------------------- Global operators --------------------------------------
  
  
  /// Computes the global Laplace-Beltrami operator by accumulating the
  /// per face operators.
  ///
  /// @param lambda the regualrization parameter for the local Laplace-Beltrami operators
  /// @return a sparse nbVertices x nbVertices matrix
  SparseMatrix globalLaplaceBeltrami(const double lambda=1.0) const
  {
    SparseMatrix lapGlobal(mySurfaceMesh->nbVertices(), mySurfaceMesh->nbVertices());
    SparseMatrix local(mySurfaceMesh->nbVertices(), mySurfaceMesh->nbVertices());
    std::vector<Triplet> triplets;
    std::vector<size_t> reorder;
    for(auto f = 0; f <  mySurfaceMesh->nbFaces(); ++f)
    {
      auto nf  = myFaceDegree[f];
      reorder.resize(nf);
      DenseMatrix Lap = this->LaplaceBeltrami(f,lambda);
      auto cpt=0;
      const auto vertices = mySurfaceMesh->incidentVertices(f);
      for(auto v: vertices )
      {
        reorder[ cpt ]= v;
        ++cpt;
      }
      
      for(auto i=0; i < nf; ++i)
        for(auto j=0; j < nf; ++j)
        {
          auto v = Lap(i,j);
          if (v!= 0.0)
            triplets.emplace_back(Triplet(reorder[i],reorder[j],Lap(i,j)));
        }
      
      local.setFromTriplets(triplets.begin(), triplets.end());
      lapGlobal += local;
      
      triplets.clear();
      local.setZero();
    }
    return lapGlobal;
  }
  
  /// Compute and returns the global lumped mass matrix
  /// (diagonal matrix with Max's weights for each vertex).
  ///    M(i,i) =   ∑_{adjface f} faceArea(f)/degree(f) ;
  ///
  /// @return the global lumped mass matrix.
  SparseMatrix globalLumpedMassMatrix() const
  {
    SparseMatrix M(mySurfaceMesh->nbVertices(), mySurfaceMesh->nbVertices());
    std::vector<Triplet> triplets;
    for(auto v=0; v < mySurfaceMesh->nbVertices(); ++v)
    {
      auto faces = mySurfaceMesh->incidentFaces(v);
      auto varea = 0.0;
      for(auto f: faces)
        varea += faceArea(f) /(double)myFaceDegree[f];
      triplets.emplace_back(Triplet(v,v,varea));
    }
    M.setFromTriplets(triplets.begin(),triplets.end());
    return M;
  }
  
  // ----------------------- Cache mechanism --------------------------------------
  
  /// Generic method to compute all the per face DenseMatrices and store them in an
  /// indexed container.
  ///
  /// Usage example:
  /// @code
  ///auto opM = [&](const PolygonalCalculus<Mesh>::Face f){ return calculus.M(f);};
  ///auto cacheM = boxCalculus.getOperatorCacheMatrix(opM);
  ///...
  /////Now you have access to the cached values and mixed them with un-cached ones
  ///  Face f = ...;
  ///  auto res = cacheM[f] * calculus.D(f) * phi;
  /// ...
  ///@endcode
  ///
  /// @param perFaceOperator the per face operator
  /// @return an indexed container of all DenseMatrix operators (indexed per Face).
  std::vector<DenseMatrix> getOperatorCacheMatrix(const std::function<DenseMatrix(Face)> &perFaceOperator) const
  {
    std::vector<DenseMatrix> cache;
    for(auto f=0; f < mySurfaceMesh->nbFaces(); ++f)
      cache.push_back(perFaceOperator(f));
    return cache;
  }
  
  /// Generic method to compute all the per face Vector and store them in an
  /// indexed container.
  ///
  /// Usage example:
  /// @code
  ///auto opCentroid = [&](const PolygonalCalculus<Mesh>::Face f){ return calculus.centroid(f);};
  ///auto cacheCentroid = boxCalculus.getOperatorCacheVector(opCentroid);
  ///...
  /////Now you have access to the cached values and mixed them with un-cached ones
  ///  Face f = ...;
  ///  auto res = calculus.P(f) * cacheCentroid[f] ;
  /// ...
  ///@endcode
  ///
  /// @param perFaceVectorOperator the per face operator
  /// @return an indexed container of all Vector quantities (indexed per Face).
  std::vector<Vector> getOperatorCacheVector(const std::function<Vector(Face)> &perFaceVectorOperator) const
  {
    std::vector<Vector> cache;
    for(auto f=0; f < mySurfaceMesh->nbFaces(); ++f)
      cache.push_back(perFaceVectorOperator(f));
    return cache;
  }

  
  // ----------------------- Common --------------------------------------
public:
  
  /// Update the internal cache structures
  /// (e.g. degree of each face).
  void init()
  {
    updateFaceDegree();
  }
  
  /// Helper to retreive the degree of the face
  /// @param f the face
  /// @return the number of vertices of the face.
  size_t faceDegree(Face f) const
  {
    return myFaceDegree[f];
  }
  
  /// @return the number of vertices of the underlying surface mesh.
  size_t nbVertices() const
  {
    return mySurfaceMesh->nbVertices();
  }
  
  /// @return the number of faces of the underlying surface mesh.
  size_t nbFaces() const
  {
    return mySurfaceMesh->nbFaces();
  }
  
  /// @returns the degree of the face f (number of vertices)
  /// @param f the face
  size_t degree(const Face f) const
  {
    return myFaceDegree[f];
  }
  
  /// @returns an pointer (ConstAlias) to the underlying
  /// SurfaceMash instance.
  ConstAlias<SurfaceMesh> getSurfaceMeshAlias() const
  {
    return mySurfaceMesh;
  }
  
  /**
   * Writes/Displays the object on an output stream.
   * @param out the output stream where the object is written.
   */
  void selfDisplay ( std::ostream & out ) const;
  
  /**
   * Checks the validity/consistency of the object.
   * @return 'true' if the object is valid, 'false' otherwise.
   */
  bool isValid() const;
  
  
  
  // ------------------------- Protected Datas ------------------------------
protected:
  
  /// Update the face degree cache
  void updateFaceDegree()
  {
    myFaceDegree.resize(mySurfaceMesh->nbFaces());
    for(auto f = 0; f <  mySurfaceMesh->nbFaces(); ++f)
    {
      auto vertices = mySurfaceMesh->incidentVertices(f);
      auto nf = vertices.size();
      myFaceDegree[f] = nf;
    }
  }
  // ------------------------- Internals ------------------------------------
private:
  
  ///Underlying SurfaceMesh
  const SurfaceMesh *mySurfaceMesh;
  
  ///Embedding function (face,vertex)->R^3 for the vertex position wrt. the face.
  std::function<RealPoint( Face, Vertex)> myEmbedder;
  
  ///Cache containing the face degree
  std::vector<size_t> myFaceDegree;
    
  
}; // end of class PolygonalCalculus


/**
 * Overloads 'operator<<' for displaying objects of class 'PolygonalCalculus'.
 * @param out the output stream where the object is written.
 * @param object the object of class 'PolygonalCalculus' to write.
 * @return the output stream after the writing.
 */
template <typename T>
std::ostream&
operator<< ( std::ostream & out, const PolygonalCalculus<T> & object );

} // namespace DGtal


///////////////////////////////////////////////////////////////////////////////
// Includes inline functions.
#include "DGtal/dec/PolygonalCalculus.ih"

//                                                                           //
///////////////////////////////////////////////////////////////////////////////
