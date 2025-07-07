# 🚨 Immediate Fix for Your CMake Issues

## Problem Summary
Your CMakeLists.txt is missing basic structure and the target name doesn't exist.

## 🎯 Quick Fix (5 minutes)

### Step 1: Run Diagnostic
Save the "Project Diagnostic Script" as `diagnostic.sh` and run it:
```bash
chmod +x diagnostic.sh
./diagnostic.sh
```

This will tell you exactly what's wrong with your project structure.

### Step 2: Fix Your CMakeLists.txt

**BACKUP your current CMakeLists.txt first:**
```bash
cp CMakeLists.txt CMakeLists.txt.backup
```

**Then replace it entirely** with the "Complete CMakeLists.txt Template" I provided above.

**Important:** On line 47 of the new CMakeLists.txt, change this:
```cmake
set(EXECUTABLE_NAME "RestaurantPOS")  # <-- CHANGE THIS IF NEEDED
```

To match your actual project name.

### Step 3: Fix Directory Structure
```bash
# Create missing directories
mkdir -p src include resources/css build

# If you have source files elsewhere, move them to src/
# If you have header files elsewhere, move them to include/
```

### Step 4: Copy Theme Files
Copy all 5 CSS files I provided to `resources/css/`:
- theme-framework.css
- theme-light.css  
- theme-dark.css
- theme-warm.css
- theme-cool.css

### Step 5: Test the Fix
```bash
cd build
cmake ..
```

## ✅ Expected Success Output

You should see:
```
-- Build type: Debug
-- Found Wt: [version]
-- Found X source files
-- Found Y header files
-- Creating executable: RestaurantPOS
-- Configuring theme system for target: RestaurantPOS
--   ✓ theme-framework.css
--   ✓ theme-light.css
--   ✓ theme-dark.css
--   ✓ theme-warm.css
--   ✓ theme-cool.css
-- Theme CSS files found: 5/5
-- Theme system configured successfully
```

Then build:
```bash
make
```

## 🔧 If You Still Get Errors

### Error: "No source files found"
- Put your .cpp files in the `src/` directory
- The diagnostic script will show you what files it found

### Error: "Target not found"  
- Change the `EXECUTABLE_NAME` in CMakeLists.txt line 47
- Or tell me your actual project name and I'll fix it

### Error: "Wt not found"
- You need to install the Wt framework first
- On Ubuntu: `sudo apt-get install witty-dev`
- On macOS: `brew install witty`

## 🚀 Test Basic Functionality

Once it builds successfully, test the theme system:

1. **Add the minimal theme test** (from the artifact I provided) to your main app file
2. **Build and run:**
   ```bash
   ./RestaurantPOS --docroot=docroot --http-port=8080
   ```
3. **Open:** `http://localhost:8080`
4. **You should see theme test buttons** that actually work!

## 📞 If You're Still Stuck

Run the diagnostic script and paste the output. I'll give you a custom fix for your exact situation.

The diagnostic will show me:
- Your actual project structure  
- What source files you have
- What's in your CMakeLists.txt
- What's missing

Then I can provide a targeted solution! 🎯