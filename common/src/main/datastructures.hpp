/******************************************************************************
 * Filename : datastructures.hpp
 * Date     : 2018 Aug 25
 * Author   : Willy Clarke (willy@clarke.no)
 * Version  : 0.0.1
 * Copyright: W. Clarke
 * License  : MIT
 * Descripti: Data structures for My Raytracing Challenge.
 ******************************************************************************/
#ifndef COMMON_DATASTRUCTURES_HPP
#define COMMON_DATASTRUCTURES_HPP

#include <iomanip>  // for setw().
#include <iostream>
#include <limits>
#include <strstream>
#include <vector>

//------------------------------------------------------------------------------
// NOTE: The assert will write to a null pointer.
#if HANDMADE_SLOW
#define debug(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#define Assert(Condition, ...)                                        \
  if (!(Condition))                                                   \
  {                                                                   \
    fprintf(stderr, "ASSERT. Function %s. Line %d\n", ##__VA_ARGS__); \
    *(volatile int *)0 = 0;                                           \
  }
#else
#define Assert(Condition, ...)
#endif

namespace ww
{
constexpr float EPSILON = 0.0035000;  // 1E27 * std::numeric_limits<float>::min();
constexpr double PI = 3.141592653589793238463;
constexpr float PI_F = 3.14159265358979f;

//------------------------------------------------------------------------------
union tup {
  tup() : X{}, Y{}, Z{}, W{} {};
  tup(float IX, float IY, float IZ, float IW) : X{IX}, Y{IY}, Z{IZ}, W{IW} {};
  tup(float IX, float IY) : X{IX}, Y{IY}, Z{}, W{} {};
  tup(float IX, float IY, float IZ) : X{IX}, Y{IY}, Z{IZ}, W{} {};
  ~tup() {}
  tup(tup const &Other)
  {
    X = Other.X;
    Y = Other.Y;
    Z = Other.Z;
    W = Other.W;
  }
  struct  //!< A tuple is initially a vector with four elements or a 3D point.
  {
    float X{};
    float Y{};
    float Z{};
    float W{};  //!< is 1.0 when tuple is point and 0.0 when tuple is a vector.
  };
  struct  //!< A tuple can also represent colors with Intensity
  {
    float R;
    float G;
    float B;
    float I;  //!< Intensity is 1.0 at max and 0.0 at pitch black.
  };
  struct  //!< A tuple is a vector with four columns.
  {
    float C[4];
  };
};  // end of union tup.

// NOTE: Use a struct to return multiple values.
struct is_invertible_return
{
  bool IsInvertible{};
  bool IsComputed{};
  float Determinant{};
};

//------------------------------------------------------------------------------
union matrix {
  matrix() : R0{}, R1{}, R2{}, R3{}, Dimension{4} {}
  matrix(tup const &Cr0, tup const &Cr1, tup const &Cr2, tup const &Cr3)
      : R0{Cr0}, R1{Cr1}, R2{Cr2}, R3{Cr3}, Dimension{4}
  {
  }
  ~matrix() {}
  matrix(matrix const &Other)
  {
    R0 = Other.R0;
    R1 = Other.R1;
    R2 = Other.R2;
    R3 = Other.R3;
    Dimension = Other.Dimension;
    ID = Other.ID;
  }

  struct  //!< A matrix can be four tuple rows
  {
    tup R0{};
    tup R1{};
    tup R2{};
    tup R3{};
    int Dimension{4};
    // NOTE: Storeage of invertible and determinant
    is_invertible_return ID{};
  };
  struct  //!< or it can be an array of 4 tuple rows.
  {
    tup R[4];
  };
};

//------------------------------------------------------------------------------
struct material
{
  float Ambient{0.1f};     //!< Typical value between 0 and 1. Non-negative.
  float Diffuse{0.9f};     //!< Typical value between 0 and 1. Non-negative.
  float Specular{0.9f};    //!< Typical value between 0 and 1. Non-negative.
  float Shininess{200.f};  //!< Typical value between 10 and 200. Non-negative.
  tup Color{1.f, 1.f, 1.f, 0.f};
};

/// ---
/// \struct base struct for the raytracing objects
/// ---
struct object
{
  tup Center{};
  material Material{};

  //!< The transform of the object, initialize to identity matrix
  matrix Transform{
      tup{1.f, 0.f, 0.f, 0.f},  //!<
      tup{0.f, 1.f, 0.f, 0.f},  //!<
      tup{0.f, 0.f, 1.f, 0.f},  //!<
      tup{0.f, 0.f, 0.f, 1.f}   //!<
  };                            //!<
  object() {}
  virtual ~object() {}
  template <typename T>
  bool isA()
  {
    return (dynamic_cast<T *>(this) != NULL);
  }
};

/// ---
/// \struct sphere
/// \brief The sphere is defined by its center and the radii.
/// \detailed By making this struct a subclass of object we are able
///           to set up pointers to objeects of different types.
/// ---
struct sphere : public object
{
  float Radius{1.f};  //!< Radius.
};

/// ---
/// \struct cube
/// \brief Placeholder/stub for an upcoming cube. For now it is used for testing
///        of object pointers.
/// ---
struct cube : public object
{
  float L{1.f};
};

typedef std::shared_ptr<object> shared_ptr_object;
/// ---
/// \struct intersection
/// \brief Connect the time t value with the object for intersection.
/// \detail The pointer to the object allows us to build a vector of
///         objects. This is the main reason for using it.
struct intersection
{
  float t{};
  shared_ptr_object pObject{};  //!< The pointer need to be cast to a valid object type.
};

/// ---
/// \struct intersections
/// \brief A collection of intersect's as defined above.
/// ---
struct intersections
{
  std::vector<intersection> vI{};
  int Count() const { return (int)vI.size(); }
};
/// ---
/// \struct ray
/// \brief A ray consist of an origint point and a vector for the direction.
/// ---
struct ray
{
  tup Origin{0.f, 0.f, 0.f, 1.f};     //!< The origin. This is a point in space.
  tup Direction{1.f, 0.f, 0.f, 0.f};  //!< The direction. This is a vector in space.
};

//------------------------------------------------------------------------------
struct canvas
{
  canvas(int IW = 10, int IH = 10) : W{IW}, H{IH} { vXY.resize(W * H); }
  canvas(canvas const &Other)
  {
    W = Other.W;
    H = Other.H;
    vXY = Other.vXY;
  }
  ~canvas() {}
  int W{};  //<! Width
  int H{};  //<! Height
  std::vector<tup> vXY{};
};

//------------------------------------------------------------------------------
struct light
{
  tup Intensity{};  //!< Includes color.
  tup Position{};
};

typedef std::shared_ptr<light> shared_ptr_light;

//------------------------------------------------------------------------------
// \struct world :
//                Contains a vector of the objects
//                Contains a vector of light source's.
//
struct world
{
  std::vector<shared_ptr_object> vPtrObjects{};
  std::vector<shared_ptr_light> vPtrLights{};
  int Count() const { return static_cast<int>(vPtrObjects.size()); }
};

//------------------------------------------------------------------------------
// \struct prepare_computation
// \brief Holds precomputed values for the point in world space where the intersection
//        occured, the eye vector (pointing back toward the eye, or camera), and the
//        normal vector.
// ---
struct prepare_computation
{
  float t{};
  bool Inside{};  //!< Set to true when the eye is inside an object. Reverses the sign of the normal vector to ensure
                  //!< correct illumination.
  shared_ptr_object pObject{};
  tup Point{};
  tup Normal{};
  tup Eye{};
};

typedef std::shared_ptr<prepare_computation> shared_ptr_prepare_computation;

//------------------------------------------------------------------------------
struct camera
{
  int HSize{160};
  int VSize{120};
  float FieldOfView{};
  float PixelSize{};
  float HalfWidth{};
  float HalfHeight{};

  //!< The transform of the object, initialize to identity matrix
  matrix Transform{
      tup{1.f, 0.f, 0.f, 0.f},  //!<
      tup{0.f, 1.f, 0.f, 0.f},  //!<
      tup{0.f, 0.f, 1.f, 0.f},  //!<
      tup{0.f, 0.f, 0.f, 1.f}   //!<
  };                            //!<
};

//------------------------------------------------------------------------------
// NOTE: Declarations.
tup Add(tup const &A, tup const &B);
tup Color(float const R, float const G, float const B);
tup Cross(tup const &A, tup const &B);

// ---
// NOTE: For an explanation of what the dot product actually is you can take a look
//     : at the description at
//     : http://betterexplained.com/articles/vector-calculus-understanding-the-dot-product .
// ---
float Dot(tup const &A, tup const &B);

// ---
// NOTE: For a discussion on how to do comparison with floating point number the following web
//     : site can be consulted:
//     https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
// ---
bool Equal(float const A, float const B);
bool Equal(tup const &A, tup const &B);
bool IsVector(tup const &Tup);
bool IsPoint(tup const &Tup);
float MagSquared(tup const &Tup);
float Mag(tup const &Tup);
tup Mul(float const S, tup const &Tup);
float Radians(float Deg);

// ---
// NOTE: Multiply is also called Hadamard or Schur product.
// ---
tup Mul(tup const A, tup const B);
tup Negate(tup const &Tup);
tup Normalize(tup const &Tup);
tup Point(float A, float B, float C);
tup Sub(tup const &A, tup const &B);
tup Vector(float A, float B, float C);

// ---
// NOTE: Canvas declarations.
// ---
void WritePixel(canvas &Canvas, int X, int Y, tup const &Color);
tup PixelAt(canvas const &Canvas, int X, int Y);
std::strstream PPMHeader(canvas const &Canvas, int X, int Y);
std::string PPMHeader(canvas const &Canvas);
void WriteToPPM(canvas const &Canvas, std::string const &Filename = "test.ppm");
int WriteToPPMFile(canvas const &Canvas, std::string const &Filename = "test.ppm");
std::shared_ptr<canvas> ReadFromPPM(std::string const &Filename = "test.ppm");

// ---
// NOTE: Matrix functions.
// ---

// ---
// \fn Cofactor A minor that possibly have had its sign changed.
// \return
// ---
float Cofactor(matrix const &M, int RemoveRow, int RemoveCol);
float Cofactor33(matrix const &M, int RemoveRow, int RemoveCol);
float Cofactor44(matrix const &M, int RemoveRow, int RemoveCol);

float Determinant22(matrix const &M);
float Determinant33(matrix const &M);
float Determinant44(matrix const &M);
float Determinant(matrix const &M);
bool Equal(matrix const &A, matrix const &B);
float Get(matrix const &M, int Row, int Col);

is_invertible_return IsInvertible(matrix const &M);

/// \fn Inverse Calculate the inverse of the matrix M.
///
/// \brief The inverse is not always possible to calculate. When inversion is
///        not possible the Zero matrix will be returned.
/// \return Inverse when possible, Zero matrix otherwise.
matrix Inverse(matrix const &M);

/// ---
/// \fn Identity matrix
/// \return Returs a 4x4 identity matrix.
/// ---
matrix I();

matrix Matrix44();
matrix Matrix44(tup const &R0, tup const &R1, tup const &R2, tup const &R3);
matrix Matrix33(tup const &R0, tup const &R1, tup const &R2);
matrix Matrix22(tup const &R0, tup const &R1);

/// ---
/// \fn Minor Calculate the determinant of a 2x2 submatrix.
/// \ brief Remove one row and column to be able to calculate the determinant
///         of a 2x2 matrix.
/// \param matrix &
/// \param RemoveRow - The row to be removed before computing the determinant.
/// \param RemoveCol - The column to be removed before computing the determinant.
/// \return Determinant of a submatrix of which a Row,Column have been removed.
float Minor(matrix const &M, int RemoveRow, int RemoveCol);

matrix Mul(matrix const &A, matrix const &B);
matrix Mul(matrix const &A, matrix const &B);
tup Mul(matrix const &A, tup const &T);
void Set(matrix &M, int Row, int Col, float Value);
matrix Transpose(matrix const &M);
matrix SubMatrix(matrix const &M, int RemoveRow, int RemoveCol);

matrix RotateX(float Alfa);
matrix RotateY(float Alfa);
matrix RotateZ(float Alfa);
matrix Scaling(float X, float Y, float Z);
matrix Shearing(float Xy, float Xz, float Yx, float Yz, float Zx, float Zy);
matrix Translation(float X, float Y, float Z);
// Combine translation, rotate and scale in one single function.
matrix TranslateScaleRotate(                   //!<
    float TransX, float TransY, float TransZ,  //!< Translation is in m(?)
    float ScaleX, float ScaleY, float ScaleZ,  //!< Scale input is unitless.
    float AlfaX, float AlfaY, float AlfaZ      //!< Input rotation in radians.
);

/// ---
/// \fn Sphere releated functions
/// ---
intersections Intersect(shared_ptr_object Sphere, ray const &Ray);

/// ---
/// \fn PtrDefaultSphere - Create a sphere and return shared pointer to this object.
/// ---
shared_ptr_object PtrDefaultSphere();

/// ---
/// Ray releated functions.
/// ---
bool Equal(intersection const &A, intersection const &B);
bool Equal(sphere const &A, sphere const &B);
bool Equal(material const &A, material const &B);
bool Equal(light const &A, light const &B);
intersection Hit(intersections const &Intersections);
intersection Intersection(float t, shared_ptr_object pObject);
intersections Intersections(intersection const &I1, intersection const &I2);
intersections &Intersections(intersections &XS, intersection const &I);
ray Mul(matrix const &M, ray const &R);
tup PositionAt(ray const &R, float t);
ray Ray(tup const &P, tup const &V);
ray Transform(ray const &R, matrix const &M);

/// ---
/// Surface normal functions
/// ---
tup NormalAt(object const &O, tup const &P);
tup Reflect(tup const &In, tup const &Normal);

/// ---
/// Light functions
/// ---
light PointLight(tup const &Position, tup const &Intensity);
tup Lighting(material const &Material,    //!<
             light const &Light,          //!<
             tup const &Position,         //!<
             tup const &vEye,             //!<
             tup const &vNormal,          //!<
             bool const InShadow = false  //!<
);

/// ---
/// World functions.
/// ---
/// \fn World - Create a default world with two spheres and a light.
world World();
intersections IntersectWorld(world const &World, ray const &Ray);
void WorldAddObject(world &W, shared_ptr_object pObject);
void WorldAddLight(world &W, shared_ptr_light pLight);

/// ---
/// Precomputation and shading functions.
/// ---
// \fn PrepareComputations
// \brief Computes the point in world space where the intersection occured.
//        Computes the Eye vector pointing back to the eye/camera.
//        Computes the Normal vector and checks if the normal vector need to
//        be reversed should the eye be inside of the object.
// \return struct with eye and normal vector and hit point.
prepare_computation PrepareComputations(intersection const &I, ray const &R);

// \fn ShadeHit
// \brief Calculates the color at the intersection captured by Comps.
// \return tup with the color.
tup ShadeHit(world const &W, prepare_computation const &Comps);

// \fn ColorAt
// \brief Intersect the given ray with the world and return the color at the resulting
//        intersection.
// \return tup with the color.
tup ColorAt(world const &World, ray const &Ray);

// \fn ViewTransform
// \brief Orient the world releative to the eye. Line everything up to get the view we want.
matrix ViewTransform(tup const &From, tup const &To, tup const &Up);

// \fn Camera - Create a default camera.
// \param HSize - Horisontal size in pixels.
// \param VSize - Vertical size in pixels.
// \param FieldOfView - Field of view in degrees.
camera Camera(int const HSize, int const VSize, float const FieldOfView);

// \fn RayForPixel - Return a ray that starts at the camera and passes through the indicated x,y.
// \param X - Horizontal coordinate
// \param Y - Vertical coordinate
// \return Ray
ray RayForPixel(camera const &C, int const Px, int const Py);

// \fn Render - Use the camera to render an image of the given world.
canvas Render(camera const &Camera, world const &World);

//------------------------------------------------------------------------------
// Shadow functions ------------------------------------------------------------
//------------------------------------------------------------------------------
bool IsShadowed(world const &World, tup const &Point);
};  // namespace ww

// ---
// NOTE: Declare the operator for inclusion elsewhere.
// ---
std::ostream &operator<<(std::ostream &stream, const ww::tup &T);
std::ostream &operator<<(std::ostream &stream, const ww::matrix &M);
std::ostream &operator<<(std::ostream &stream, const ww::material &M);
std::ostream &operator<<(std::ostream &stream, const ww::sphere &S);
ww::tup operator+(ww::tup const &A, ww::tup const &B);
ww::tup operator-(ww::tup const &Tup);
ww::tup operator-(ww::tup const &A, ww::tup const &B);
ww::tup operator*(float const S, ww::tup const &Tup);
ww::tup operator*(ww::tup const &Tup, float const S);
ww::tup operator*(ww::tup const &A, ww::tup const &B);
ww::tup operator/(ww::tup const &Tup, float const S);
ww::matrix operator*(ww::matrix const &A, ww::matrix const &B);
ww::tup operator*(ww::matrix const &A, ww::tup const &T);
ww::ray operator*(ww::matrix const &M, ww::ray const &R);
bool operator==(ww::intersection const &A, ww::intersection const &B);
bool operator==(ww::light const &A, ww::light const &B);
bool operator==(ww::material const &A, ww::material const &B);
bool operator==(ww::matrix const &A, ww::matrix const &B);
bool operator==(ww::sphere const &A, ww::sphere const &B);
bool operator==(ww::tup const &A, ww::tup const &B);
#endif
