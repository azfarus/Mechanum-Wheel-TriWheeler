import numpy as np

# Define the matrix
A = np.array([       
                [np.cos(np.deg2rad(-45)), np.sin(np.deg2rad(-45))],
                [np.cos(np.deg2rad(30+45)), np.sin(np.deg2rad(30+45))],
                [np.cos(np.deg2rad(150+45)), np.sin(np.deg2rad(150+45))]
            ] 
    )

# Compute the pseudoinverse
A_pinv = np.linalg.pinv(A)

# Display the result
print("Original Matrix:")
print(A)
print("\nPseudoinverse:")
print(A_pinv)
