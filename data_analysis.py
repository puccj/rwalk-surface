import numpy as np

def geodesic_distance(a, b, radius=4.5, output='rad'):
    """
    Compute the geodesic (great-circle) distance between two 3D points on a sphere.

    Parameters
    ----------
    a, b : array-like, shape (3,)
        3D position vectors of the two points (do not need to be unit length).
    radius : float, optional
        Radius of the sphere. Default is 4.5.
    output : {'rad', 'deg', 'arc'}, optional
        If 'rad', return the distance in radians. If 'deg', return the distance in degrees.
        If 'arc', return arc length (same units as `radius`). Default is 'rad'.

    Returns
    -------
    float
        Geodesic distance (arc length if degrees=False, angular distance if degrees=True).
    """
    a = np.asarray(a, dtype=float)
    b = np.asarray(b, dtype=float)

    # Normalize to unit vectors
    na = np.linalg.norm(a)
    nb = np.linalg.norm(b)
    if na == 0 or nb == 0:
        raise ValueError("Input vectors must be nonzero.")
    a_unit = a / na
    b_unit = b / nb

    # Compute angle between vectors
    dot = np.clip(np.dot(a_unit, b_unit), -1.0, 1.0) # dot product clamped to [-1, 1]
    cross_norm = np.linalg.norm(np.cross(a_unit, b_unit))
    angle = np.arctan2(cross_norm, dot)  # radians in [0, π]

    if output == 'deg':
        return np.degrees(angle)
    elif output == 'arc':
        return radius * angle
    elif output == 'rad':
        return angle
    else:
        raise ValueError("Invalid output type. Choose from 'rad', 'deg', or 'arc'.")
    
def geodesic_distances(points, starting_point=(0,0,4.5), radius=4.5, output='rad'):
    """
    Compute geodesic distances from a starting point to multiple points on a sphere (vectorized).

    Parameters
    ----------
    points : array-like, shape (N, 3)
        3D position vectors of the points.
    starting_point : array-like, shape (3,), optional
        3D position vector of the starting point. Default is (0, 0, radius).
    radius : float, optional
        Radius of the sphere. Default is 4.5.
    output : {'rad', 'deg', 'arc'}, optional
        If 'rad', return distances in radians. If 'deg', return distances in degrees. 
        If 'arc', return arc lengths. Default is 'rad'.

    Returns
    -------
    distances : ndarray, shape (N,)
        Geodesic distances from the starting point to each point.
    """
    points = np.asarray(points)
    starting_point = np.asarray(starting_point)

    points_unit = points / np.linalg.norm(points, axis=1)[:, None]
    start_unit = starting_point / np.linalg.norm(starting_point)

    # compute the dot products of each row of points_unit with start_unit, producing a 1D array
    dots = np.einsum('ij,j->i', points_unit, start_unit)
    dots = np.clip(dots, -1.0, 1.0)  # clip to avoid numerical issues

    cross_norms = np.linalg.norm(np.cross(points_unit, start_unit), axis=1)
    angles = np.arctan2(cross_norms, dots)  # radians
    
    if output == 'deg':
        return np.degrees(angles)
    elif output == 'arc':
        return radius * angles
    elif output == 'rad':
        return angles
    else:
        raise ValueError("Invalid output type. Choose from 'rad', 'deg', or 'arc'.")

def test_geodesic_distance():
    # Check that both functions give the same result
    # generate random points and compute distance using the two functions, comparing results and timing

    import time
    num_tests = 10000
    points = np.random.randn(num_tests, 3) * 4.5
    starting_point = np.array([0,0,4.5])

    start_time = time.time()
    distances1 = [geodesic_distance(p, starting_point, radius=4.5) for p in points]
    time1 = time.time() - start_time

    start_time = time.time()
    distances2 = geodesic_distances(points, starting_point, radius=4.5)
    time2 = time.time() - start_time

    print(f"geodesic_distance time: {time1:.4f} s")
    print(f"variance time: {time2:.4f} s")
    print(f"Max difference between methods: {np.max(np.abs(np.array(distances1) - distances2)):.30f}")

def variance(final_positions, starting_point=(0,0,4.5), radius=4.5):
    """
    Compute the variance of geodesic distances from a starting point to multiple final positions on a sphere.
    """
    angles = geodesic_distances(final_positions, starting_point, radius=radius, output='rad')
    var = np.mean(angles**2)
    return var