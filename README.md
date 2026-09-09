# adaptive-ai-compiler-runtime
An adaptive AI compiler and runtime for computation graph optimization, operator fusion, quantization, and CPU-GPU execution scheduling.

# Repo structure
adaptive-ai-compiler-runtime/
│
├── graph/
│   └── graph.cpp
│
└── README.md

# current work

AI Model
   ↓
Computation Graph
   ↓
Dependency Analysis
   ↓
Topological Ordering       ← IAM  HERE
   ↓
Optimization
   ↓
Kernel Selection
   ↓
Runtime Scheduler
   ↓
CPU / GPU
