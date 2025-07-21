# Computer Architecture Projects in C

This repository gathers five lab-style projects completed for UT Austin’s CS 429 course. Each lab explores a fundamental topic in the hardware–software interface, implemented entirely in C and accompanied by makefiles and test drivers.

## Projects

| Folder | Focus | What You’ll See |
|--------|-------|-----------------|
| **CacheLab** | Cache performance and locality | A simulator that parses memory traces and reports hit/miss statistics for direct-mapped and set-associative caches. |
| **CacheIndexingLab** | Address decomposition | Bit-level manipulation of tag, index, and offset fields to show how addresses map to cache lines. |
| **MallocLab** | Dynamic memory allocation | A custom `malloc`/`free`/`realloc` implementation using explicit free lists and boundary tags, with throughput/fragmentation evaluation. |
| **PipelineControlLab** | Five-stage CPU pipeline | Instruction-level simulator with hazard detection, forwarding logic, and control-flow stall handling. |
| **SpeculativeExecutionLab** | Branch prediction and speculation | Models of branch predictors and speculative execution structures to study performance vs. correctness. |

## Development Notes

* **Incremental checkpoints:** Each lab was built in stages with automated runners validating new requirements.
* **Collaboration:** Some projects are individually done. Some projects allowed collaboration.
* **Debugging:** ChatGPT was occasionally used to assist with tracing segmentation faults or logic errors (e.g., prompts like “Please debug my C code”).

## Building and Running

Each folder includes its own `Makefile`:

```bash
cd CacheLab
make
./csim -t traces/trace1.txt
```

See individual folders for any additional instructions or test inputs.

## License

Unless otherwise noted, the code is released under the MIT License.
