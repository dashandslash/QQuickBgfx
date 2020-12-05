#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  public:
	virtual ~Camera() {}

	//! Returns the position in world-space from which the Camera is viewing
	glm::vec3		getEyePoint() const { return mEyePoint; }
	//! Sets the position in world-space from which the Camera is viewing
	void		setEyePoint( const glm::vec3 &eyePoint );

	//! Returns the vector in world-space which represents "up" - typically glm::vec3( 0, 1, 0 )
	glm::vec3		getWorldUp() const { return mWorldUp; }
	//! Sets the vector in world-space which represents "up" - typically glm::vec3( 0, 1, 0 )
	void		setWorldUp( const glm::vec3 &worldUp );

	//! Modifies the view direction to look from the current eyePoint to \a target. Also updates the pivot distance.
	void		lookAt( const glm::vec3 &target );
	//! Modifies the eyePoint and view direction to look from \a eyePoint to \a target. Also updates the pivot distance.
	void		lookAt( const glm::vec3 &eyePoint, const glm::vec3 &target );
	//! Modifies the eyePoint and view direction to look from \a eyePoint to \a target with up vector \a up (to achieve camera roll). Also updates the pivot distance.
	void		lookAt( const glm::vec3 &eyePoint, const glm::vec3 &target, const glm::vec3 &up );
	//! Returns the world-space vector along which the camera is oriented
	glm::vec3		getViewDirection() const { return mViewDirection; }
	//! Sets the world-space vector along which the camera is oriented
	void		setViewDirection( const glm::vec3 &viewDirection );

	//! Returns the world-space quaternion that expresses the camera's orientation
	glm::quat		getOrientation() const { return mOrientation; }
	//! Sets the camera's orientation with world-space quaternion \a orientation
	void		setOrientation( const glm::quat &orientation );

	//! Returns the camera's vertical field of view measured in degrees.
	float	getFov() const { return mFov; }
	//! Sets the camera's vertical field of view measured in degrees.
	void	setFov( float verticalFov ) { mFov = verticalFov;  mProjectionCached = false; }
	//! Returns the camera's horizontal field of view measured in degrees.
	float	getFovHorizontal() const { return glm::degrees( 2.0f * atan( tan( glm::radians(mFov) * 0.5f ) * mAspectRatio ) ); }
	//! Sets the camera's horizontal field of view measured in degrees.
	void	setFovHorizontal( float horizontalFov ) { mFov = glm::degrees( 2.0f * atan( tan( glm::radians( horizontalFov ) * 0.5f ) / mAspectRatio ) );  mProjectionCached = false; }
	//! Returns the camera's focal length, calculating it based on the field of view.
	float	getFocalLength() const;

	//! Primarily for user interaction, such as with CameraUi. Returns the distance from the camera along the view direction relative to which tumbling and dollying occur.
	float	getPivotDistance() const { return mPivotDistance; }
	//! Primarily for user interaction, such as with CameraUi. Sets the distance from the camera along the view direction relative to which tumbling and dollying occur.
	void	setPivotDistance( float distance ) { mPivotDistance = distance; }
	//! Primarily for user interaction, such as with CameraUi. Returns the world-space point relative to which tumbling and dollying occur.
	glm::vec3	getPivotPoint() const { return mEyePoint + mViewDirection * mPivotDistance; }

	//! Returns the aspect ratio of the image plane - its width divided by its height
	float	getAspectRatio() const { return mAspectRatio; }
	//! Sets the aspect ratio of the image plane - its width divided by its height
	void	setAspectRatio( float aAspectRatio ) { mAspectRatio = aAspectRatio; mProjectionCached = false; }
	//! Returns the distance along the view direction to the Near clipping plane.
	float	getNearClip() const { return mNearClip; }
	//! Sets the distance along the view direction to the Near clipping plane.
	void	setNearClip( float nearClip ) { mNearClip = nearClip; mProjectionCached = false; }
	//! Returns the distance along the view direction to the Far clipping plane.
	float	getFarClip() const { return mFarClip; }
	//! Sets the distance along the view direction to the Far clipping plane.
	void	setFarClip( float farClip ) { mFarClip = farClip; mProjectionCached = false; }

	//! Returns the four corners of the Camera's Near clipping plane, expressed in world-space
	virtual void	getNearClipCoordinates( glm::vec3 *topLeft, glm::vec3 *topRight, glm::vec3 *bottomLeft, glm::vec3 *bottomRight ) const { return getClipCoordinates( mNearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }
	//! Returns the four corners of the Camera's Far clipping plane, expressed in world-space
	virtual void	getFarClipCoordinates( glm::vec3 *topLeft, glm::vec3 *topRight, glm::vec3 *bottomLeft, glm::vec3 *bottomRight ) const { getClipCoordinates( mFarClip, mFarClip / mNearClip, topLeft, topRight, bottomLeft, bottomRight ); }

	//! Returns the coordinates of the camera's frustum, suitable for passing to \c glFrustum
	void	getFrustum( float *left, float *top, float *right, float *bottom, float *near, float *far ) const;
	//! Returns whether the camera represents a perspective projection instead of an orthographic
	virtual bool isPersp() const = 0;
	
	//! Returns the Camera's Projection matrix, which converts view-space into clip-space
	virtual const glm::mat4&	getProjectionMatrix() const { if( ! mProjectionCached ) calcProjection(); return mProjectionMatrix; }
	//! Returns the Camera's View matrix, which converts world-space into view-space
	virtual const glm::mat4&	getViewMatrix() const { if( ! mModelViewCached ) calcViewMatrix(); return mViewMatrix; }
	//! Returns the Camera's Inverse View matrix, which converts view-space into world-space
	virtual const glm::mat4&	getInverseViewMatrix() const { if( ! mInverseModelViewCached ) calcInverseView(); return mInverseModelViewMatrix; }

	//! Returns a Ray that passes through the image plane coordinates (\a u, \a v) (expressed in the range [0,1]) on an image plane of aspect ratio \a imagePlaneAspectRatio
//	Ray		generateRay( float u, float v, float imagePlaneAspectRatio ) const { return calcRay( u, v, imagePlaneAspectRatio ); }
	//! Returns a Ray that passes through the pixels coordinates \a posPixels on an image of size \a imageSizePixels
//	Ray		generateRay( const glm::vec2 &posPixels, const glm::vec2 &imageSizePixels ) const { return calcRay( posPixels.x / imageSizePixels.x, ( imageSizePixels.y - posPixels.y ) / imageSizePixels.y, imageSizePixels.x / imageSizePixels.y ); }
	//! Returns the \a right and \a up vectors suitable for billboarding relative to the Camera
	void	getBillboardVectors( glm::vec3 *right, glm::vec3 *up ) const;

	//! Converts a world-space coordinate \a worldCoord to screen coordinates as viewed by the camera, based on a screen which is \a screenWidth x \a screenHeight pixels.
	glm::vec2 worldToScreen( const glm::vec3 &worldCoord, float screenWidth, float screenHeight ) const;
	//! Converts a eye-space coordinate \a eyeCoord to screen coordinates as viewed by the camera
	glm::vec2 eyeToScreen( const glm::vec3 &eyeCoord, const glm::vec2 &screenSizePixels ) const;
	//! Converts a world-space coordinate \a worldCoord to eye-space, also known as camera-space. -Z is along the view direction.
	glm::vec3 worldToEye( const glm::vec3 &worldCoord ) const	{ return glm::vec3( getViewMatrix() * glm::vec4( worldCoord, 1 ) ); }
	//! Converts a world-space coordinate \a worldCoord to the z axis of eye-space, also known as camera-space. -Z is along the view direction. Suitable for depth sorting.
	float worldToEyeDepth( const glm::vec3 &worldCoord ) const;
	//! Converts a world-space coordinate \a worldCoord to normalized device coordinates
	glm::vec3 worldToNdc( const glm::vec3 &worldCoord ) const;

	//! Calculates the area of the screen-space elliptical projection of \a sphere
//	float	calcScreenArea( const Sphere &sphere, const glm::vec2 &screenSizePixels ) const;
	//! Calculates the screen-space elliptical projection of \a sphere, putting the results in \a outCenter, \a outAxisA and \a outAxisB
//	void	calcScreenProjection( const Sphere &sphere, const glm::vec2 &screenSizePixels, glm::vec2 *outCenter, glm::vec2 *outAxisA, glm::vec2 *outAxisB ) const;

  protected:
	Camera()
		: mWorldUp( glm::vec3( 0, 1, 0 ) ), mFov( 35 ), mAspectRatio( 1 ), mNearClip( 0.1f ), mFarClip( 1000 )
		, mPivotDistance( 1 ), mProjectionCached( false ), mModelViewCached( false ), mInverseModelViewCached( false )
		, mFrustumLeft( -1 ), mFrustumRight( 1 ), mFrustumTop( 1 ), mFrustumBottom( -1 )
	{
	}

	void			calcMatrices() const;

	virtual void	calcViewMatrix() const;
	virtual void	calcInverseView() const;
	virtual void	calcProjection() const = 0;

//	virtual Ray		calcRay( float u, float v, float imagePlaneAspectRatio ) const = 0;

	void			getClipCoordinates( float clipDist, float ratio, glm::vec3* topLeft, glm::vec3* topRight, glm::vec3* bottomLeft, glm::vec3* bottomRight ) const;

	glm::vec3	mEyePoint;
	glm::vec3	mViewDirection;
	glm::quat	mOrientation;
	glm::vec3	mWorldUp;

	float	mFov; // vertical field of view in degrees
	float	mAspectRatio;
	float	mNearClip;
	float	mFarClip;
	float	mPivotDistance;

	mutable glm::vec3	mU;	// Right vector
	mutable glm::vec3	mV;	// Readjust up-vector
	mutable glm::vec3	mW;	// Negative view direction

	mutable glm::mat4	mProjectionMatrix, mInverseProjectionMatrix;
	mutable bool	mProjectionCached;
	mutable glm::mat4	mViewMatrix;
	mutable bool	mModelViewCached;
	mutable glm::mat4	mInverseModelViewMatrix;
	mutable bool	mInverseModelViewCached;
	
	mutable float	mFrustumLeft, mFrustumRight, mFrustumTop, mFrustumBottom;
};

//! A perspective Camera.
class CameraPersp : public Camera {
  public:
	//! Creates a default camera with eyePoint at ( 28, 21, 28 ), looking at the origin, 35deg vertical field-of-view and a 1.333 aspect ratio.
	CameraPersp();
	//! Constructs screen-aligned camera
	CameraPersp( int pixelWidth, int pixelHeight, float fov );
	//! Constructs screen-aligned camera
	CameraPersp( int pixelWidth, int pixelHeight, float fov, float nearPlane, float farPlane );

	//! Configures the camera's projection according to the provided parameters.
	void	setPerspective( float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane );
	
	/** Returns both the horizontal and vertical lens shift.
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	getLensShift( float *horizontal, float *vertical ) const { *horizontal = mLensShift.x; *vertical = mLensShift.y; }
	/** Returns both the horizontal and vertical lens shift.
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	glm::vec2	getLensShift() const { return glm::vec2( mLensShift.x, mLensShift.y ); }
	/** Sets both the horizontal and vertical lens shift.
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShift( float horizontal, float vertical );
	/** Sets both the horizontal and vertical lens shift.
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShift( const glm::vec2 &shift ) { setLensShift( shift.x, shift.y ); }
	//! Returns the horizontal lens shift. A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
	float	getLensShiftHorizontal() const { return mLensShift.x; }
	/** Sets the horizontal lens shift.
		A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport. */
	void	setLensShiftHorizontal( float horizontal ) { setLensShift( horizontal, mLensShift.y ); }
	//! Returns the vertical lens shift. A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport.
	float	getLensShiftVertical() const { return mLensShift.y; }
	/** Sets the vertical lens shift.
		A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
	void	setLensShiftVertical( float vertical ) { setLensShift( mLensShift.x, vertical ); }
	
	bool	isPersp() const override { return true; }

	//! Returns a Camera whose eyePoint is positioned to exactly frame \a worldSpaceSphere but is equivalent in other parameters (including orientation). Sets the result's pivotDistance to be the distance to \a worldSpaceSphere's center.
//	CameraPersp		calcFraming( const Sphere &worldSpaceSphere ) const;

	//! Returns a subdivided portion of this camera's view frustrum as a new CameraPersp; useful for multi-gpu or tiled-rendering for instance.
//	CameraPersp		subdivide( const glm::uglm::vec2& gridSize, const glm::uglm::vec2& gridIndex ) const;
  protected:
	glm::vec2	mLensShift;

	void	calcProjection() const override;
//	Ray		calcRay( float u, float v, float imagePlaneAspectRatio ) const override;
};
