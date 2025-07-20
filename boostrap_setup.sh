# Create Bootstrap resources directory structure
mkdir -p resources/themes/bootstrap5

# Download Bootstrap 5.3.0 resources
echo "Downloading Bootstrap CSS..."
curl -o resources/themes/bootstrap5/bootstrap.min.css \
    https://cdnjs.cloudflare.com/ajax/libs/bootstrap/5.3.0/css/bootstrap.min.css

echo "Downloading Bootstrap JS..."
curl -o resources/themes/bootstrap5/bootstrap.bundle.min.js \
    https://cdnjs.cloudflare.com/ajax/libs/bootstrap/5.3.0/js/bootstrap.bundle.min.js

# Create any additional required resource files
echo "/* Bootstrap theme resources */" > resources/themes/bootstrap5/bootstrap.css

echo "Bootstrap resources downloaded successfully!"
echo "Directory structure:"
ls -la resources/themes/bootstrap5/
