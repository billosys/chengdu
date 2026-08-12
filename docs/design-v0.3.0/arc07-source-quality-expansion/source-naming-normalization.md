# Arc07 Source Naming Normalization

Status: Slice03 implementation report
Platform evidence: macos-arm64

## Accepted naming policy

First-party maintained C/C++ source files and first-party source directories use lower snake case basenames. The check accepts lowercase ASCII letters, digits, and single underscores in the stem. Acronym clusters are normalized as words when the mapping is clear, for example `hddl_writer`, `h2_mutexes`, `int_data_structures`, and `progression_network`.

Conventional build filenames such as `CMakeLists.txt`, `Makefile`, and `makefile` are exempt. C++ identifiers, class names, CMake target names, public binary names, and managed-process behavior are not renamed by this policy.

## Source-class exclusions

The excluded paths are not silently promoted into first-party maintained naming scope:

- `GENERATED` generated output such as Flex/Bison/gengetopt `cmdline.c`, `cmdline.h`, `hddl.cpp`, `hddl.hpp`, and `hddl-token.cpp`.
- `GENERATOR` inputs such as `hddl.y`, `hddl-token.l`, and `options.ggo`; they are source-classed separately from maintained C/C++ files.
- `THIRD_PARTY` and `DEPENDENCY` roots, including `pandaPI/grounder/cpddl`, `pandaPI/grounder/h2-fd-preprocessor`, nested cpddl third-party source, and `pandaPI/engine/src/symbolic_search/cudd-3.0.0`.
- `COPIED_BUILD` and `BUILD_OUTPUT` roots under `build/`, `dist/`, and `release/`.

## Initial inventory summary

Slice03 normalized the non-conforming first-party maintained C/C++ path surface identified after Slice02. Initial violations were concentrated in parser writer files, grounder helper/model files, and engine root, heuristic, landmark, data-structure, search, and translation files/directories.

| Current checked surface | Count |
|-------------------------|-------|
| first-party maintained C/C++ files | 233 |
| first-party source directories | 20 |
| current naming violations | 0 |
| final rename map entries | 132 |

Generated evidence is written to `build/source-quality/macos-arm64/source-quality-naming-violations.tsv`.

## Final rename map

| Old path | New path |
|----------|----------|
| `pandaPI/parser/src/hddlWriter.cpp` | `pandaPI/parser/src/hddl_writer.cpp` |
| `pandaPI/parser/src/hddlWriter.hpp` | `pandaPI/parser/src/hddl_writer.hpp` |
| `pandaPI/parser/src/hpdlWriter.cpp` | `pandaPI/parser/src/hpdl_writer.cpp` |
| `pandaPI/parser/src/hpdlWriter.hpp` | `pandaPI/parser/src/hpdl_writer.hpp` |
| `pandaPI/parser/src/htn2stripsWriter.cpp` | `pandaPI/parser/src/htn2strips_writer.cpp` |
| `pandaPI/parser/src/htn2stripsWriter.hpp` | `pandaPI/parser/src/htn2strips_writer.hpp` |
| `pandaPI/parser/src/orderingDecomposition.cpp` | `pandaPI/parser/src/ordering_decomposition.cpp` |
| `pandaPI/parser/src/orderingDecomposition.hpp` | `pandaPI/parser/src/ordering_decomposition.hpp` |
| `pandaPI/parser/src/shopWriter.cpp` | `pandaPI/parser/src/shop_writer.cpp` |
| `pandaPI/parser/src/shopWriter.hpp` | `pandaPI/parser/src/shop_writer.hpp` |
| `pandaPI/grounder/src/FAMmutexes.cpp` | `pandaPI/grounder/src/fam_mutexes.cpp` |
| `pandaPI/grounder/src/FAMmutexes.h` | `pandaPI/grounder/src/fam_mutexes.h` |
| `pandaPI/grounder/src/givenPlan.cpp` | `pandaPI/grounder/src/given_plan.cpp` |
| `pandaPI/grounder/src/givenPlan.h` | `pandaPI/grounder/src/given_plan.h` |
| `pandaPI/grounder/src/groundedGPG.cpp` | `pandaPI/grounder/src/grounded_gpg.cpp` |
| `pandaPI/grounder/src/groundedGPG.h` | `pandaPI/grounder/src/grounded_gpg.h` |
| `pandaPI/grounder/src/h2mutexes.cpp` | `pandaPI/grounder/src/h2_mutexes.cpp` |
| `pandaPI/grounder/src/h2mutexes.h` | `pandaPI/grounder/src/h2_mutexes.h` |
| `pandaPI/grounder/src/hierarchy-typing.cpp` | `pandaPI/grounder/src/hierarchy_typing.cpp` |
| `pandaPI/grounder/src/hierarchy-typing.h` | `pandaPI/grounder/src/hierarchy_typing.h` |
| `pandaPI/grounder/src/liftedGPG.cpp` | `pandaPI/grounder/src/lifted_gpg.cpp` |
| `pandaPI/grounder/src/liftedGPG.h` | `pandaPI/grounder/src/lifted_gpg.h` |
| `pandaPI/grounder/src/naiveGrounding.cpp` | `pandaPI/grounder/src/naive_grounding.cpp` |
| `pandaPI/grounder/src/naiveGrounding.h` | `pandaPI/grounder/src/naive_grounding.h` |
| `pandaPI/engine/src/Debug.cpp` | `pandaPI/engine/src/debug.cpp` |
| `pandaPI/engine/src/Debug.h` | `pandaPI/engine/src/debug.h` |
| `pandaPI/engine/src/Invariants.cpp` | `pandaPI/engine/src/invariants.cpp` |
| `pandaPI/engine/src/Invariants.h` | `pandaPI/engine/src/invariants.h` |
| `pandaPI/engine/src/Model.cpp` | `pandaPI/engine/src/model.cpp` |
| `pandaPI/engine/src/Model.h` | `pandaPI/engine/src/model.h` |
| `pandaPI/engine/src/ProgressionNetwork.cpp` | `pandaPI/engine/src/progression_network.cpp` |
| `pandaPI/engine/src/ProgressionNetwork.h` | `pandaPI/engine/src/progression_network.h` |
| `pandaPI/engine/src/SearchEngine.cpp` | `pandaPI/engine/src/search_engine.cpp` |
| `pandaPI/engine/src/Util.cpp` | `pandaPI/engine/src/util.cpp` |
| `pandaPI/engine/src/Util.h` | `pandaPI/engine/src/util.h` |
| `pandaPI/engine/src/VisitedList.cpp` | `pandaPI/engine/src/visited_list.cpp` |
| `pandaPI/engine/src/VisitedList.h` | `pandaPI/engine/src/visited_list.h` |
| `pandaPI/engine/src/interactivePlanner.cpp` | `pandaPI/engine/src/interactive_planner.cpp` |
| `pandaPI/engine/src/interactivePlanner.h` | `pandaPI/engine/src/interactive_planner.h` |
| `pandaPI/engine/src/heuristics/dofHeuristics` | `pandaPI/engine/src/heuristics/dof_heuristics` |
| `pandaPI/engine/src/heuristics/dof_heuristics/hhDOfree.cpp` | `pandaPI/engine/src/heuristics/dof_heuristics/hh_do_free.cpp` |
| `pandaPI/engine/src/heuristics/dof_heuristics/hhDOfree.h` | `pandaPI/engine/src/heuristics/dof_heuristics/hh_do_free.h` |
| `pandaPI/engine/src/heuristics/dof_heuristics/hhStatisticsCollector.cpp` | `pandaPI/engine/src/heuristics/dof_heuristics/hh_statistics_collector.cpp` |
| `pandaPI/engine/src/heuristics/dof_heuristics/hhStatisticsCollector.h` | `pandaPI/engine/src/heuristics/dof_heuristics/hh_statistics_collector.h` |
| `pandaPI/engine/src/heuristics/Heuristic.cpp` | `pandaPI/engine/src/heuristics/heuristic.cpp` |
| `pandaPI/engine/src/heuristics/Heuristic.h` | `pandaPI/engine/src/heuristics/heuristic.h` |
| `pandaPI/engine/src/heuristics/HeuristicPayload.cpp` | `pandaPI/engine/src/heuristics/heuristic_payload.cpp` |
| `pandaPI/engine/src/heuristics/HeuristicPayload.h` | `pandaPI/engine/src/heuristics/heuristic_payload.h` |
| `pandaPI/engine/src/heuristics/hhSimple.cpp` | `pandaPI/engine/src/heuristics/hh_simple.cpp` |
| `pandaPI/engine/src/heuristics/hhSimple.h` | `pandaPI/engine/src/heuristics/hh_simple.h` |
| `pandaPI/engine/src/heuristics/hhZero.cpp` | `pandaPI/engine/src/heuristics/hh_zero.cpp` |
| `pandaPI/engine/src/heuristics/hhZero.h` | `pandaPI/engine/src/heuristics/hh_zero.h` |
| `pandaPI/engine/src/heuristics/planningGraph.cpp` | `pandaPI/engine/src/heuristics/planning_graph.cpp` |
| `pandaPI/engine/src/heuristics/planningGraph.h` | `pandaPI/engine/src/heuristics/planning_graph.h` |
| `pandaPI/engine/src/heuristics/landmarks/hhLMCount.cpp` | `pandaPI/engine/src/heuristics/landmarks/hh_lm_count.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/hhLMCount.h` | `pandaPI/engine/src/heuristics/landmarks/hh_lm_count.h` |
| `pandaPI/engine/src/heuristics/landmarks/hhLMPayload.cpp` | `pandaPI/engine/src/heuristics/landmarks/hh_lm_payload.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/hhLMPayload.h` | `pandaPI/engine/src/heuristics/landmarks/hh_lm_payload.h` |
| `pandaPI/engine/src/heuristics/landmarks/lmDataStructures` | `pandaPI/engine/src/heuristics/landmarks/lm_data_structures` |
| `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/LmMap.cpp` | `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/lm_map.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/LmMap.h` | `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/lm_map.h` |
| `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/lookUpTab.cpp` | `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/look_up_tab.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/lookUpTab.h` | `pandaPI/engine/src/heuristics/landmarks/lm_data_structures/look_up_tab.h` |
| `pandaPI/engine/src/heuristics/landmarks/lmExtraction` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LMsInAndOrGraphs.cpp` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lms_in_and_or_graphs.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LMsInAndOrGraphs.h` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lms_in_and_or_graphs.h` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LmAoNode.cpp` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lm_ao_node.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LmAoNode.h` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lm_ao_node.h` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LmCausal.cpp` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lm_causal.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LmCausal.h` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lm_causal.h` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LmFdConnector.cpp` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lm_fd_connector.cpp` |
| `pandaPI/engine/src/heuristics/landmarks/lm_extraction/LmFdConnector.h` | `pandaPI/engine/src/heuristics/landmarks/lm_extraction/lm_fd_connector.h` |
| `pandaPI/engine/src/heuristics/rcHeuristics` | `pandaPI/engine/src/heuristics/rc_heuristics` |
| `pandaPI/engine/src/heuristics/rc_heuristics/LMCutLandmark.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/lm_cut_landmark.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/LMCutLandmark.h` | `pandaPI/engine/src/heuristics/rc_heuristics/lm_cut_landmark.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/RCModelFactory.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/rc_model_factory.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/RCModelFactory.h` | `pandaPI/engine/src/heuristics/rc_heuristics/rc_model_factory.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hhRC.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/hh_rc.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hhRC.h` | `pandaPI/engine/src/heuristics/rc_heuristics/hh_rc.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hhRC2.h` | `pandaPI/engine/src/heuristics/rc_heuristics/hh_rc2.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsAddFF.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_add_ff.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsAddFF.h` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_add_ff.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsAddFF-2.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_add_ff_2.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsAddFF-2.h` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_add_ff_2.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsFilter.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_filter.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsFilter.h` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_filter.h` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsLmCut.cpp` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_lm_cut.cpp` |
| `pandaPI/engine/src/heuristics/rc_heuristics/hsLmCut.h` | `pandaPI/engine/src/heuristics/rc_heuristics/hs_lm_cut.h` |
| `pandaPI/engine/src/intDataStructures` | `pandaPI/engine/src/int_data_structures` |
| `pandaPI/engine/src/int_data_structures/CompressedSequenceSet.cpp` | `pandaPI/engine/src/int_data_structures/compressed_sequence_set.cpp` |
| `pandaPI/engine/src/int_data_structures/CompressedSequenceSet.h` | `pandaPI/engine/src/int_data_structures/compressed_sequence_set.h` |
| `pandaPI/engine/src/int_data_structures/FlexIntStack.cpp` | `pandaPI/engine/src/int_data_structures/flex_int_stack.cpp` |
| `pandaPI/engine/src/int_data_structures/FlexIntStack.h` | `pandaPI/engine/src/int_data_structures/flex_int_stack.h` |
| `pandaPI/engine/src/int_data_structures/HashTable.cpp` | `pandaPI/engine/src/int_data_structures/hash_table.cpp` |
| `pandaPI/engine/src/int_data_structures/HashTable.h` | `pandaPI/engine/src/int_data_structures/hash_table.h` |
| `pandaPI/engine/src/int_data_structures/IntPairHeap-2.cpp` | `pandaPI/engine/src/int_data_structures/int_pair_heap_2.cpp` |
| `pandaPI/engine/src/int_data_structures/IntPairHeap-2.h` | `pandaPI/engine/src/int_data_structures/int_pair_heap_2.h` |
| `pandaPI/engine/src/int_data_structures/IntPairHeap.h` | `pandaPI/engine/src/int_data_structures/int_pair_heap.h` |
| `pandaPI/engine/src/int_data_structures/IntStack.cpp` | `pandaPI/engine/src/int_data_structures/int_stack.cpp` |
| `pandaPI/engine/src/int_data_structures/IntStack.h` | `pandaPI/engine/src/int_data_structures/int_stack.h` |
| `pandaPI/engine/src/int_data_structures/IntUtil.cpp` | `pandaPI/engine/src/int_data_structures/int_util.cpp` |
| `pandaPI/engine/src/int_data_structures/IntUtil.h` | `pandaPI/engine/src/int_data_structures/int_util.h` |
| `pandaPI/engine/src/int_data_structures/SequenceSet.cpp` | `pandaPI/engine/src/int_data_structures/sequence_set.cpp` |
| `pandaPI/engine/src/int_data_structures/SequenceSet.h` | `pandaPI/engine/src/int_data_structures/sequence_set.h` |
| `pandaPI/engine/src/int_data_structures/SequenceSetCommon.h` | `pandaPI/engine/src/int_data_structures/sequence_set_common.h` |
| `pandaPI/engine/src/int_data_structures/StringUtil.cpp` | `pandaPI/engine/src/int_data_structures/string_util.cpp` |
| `pandaPI/engine/src/int_data_structures/StringUtil.h` | `pandaPI/engine/src/int_data_structures/string_util.h` |
| `pandaPI/engine/src/int_data_structures/bIntSet.cpp` | `pandaPI/engine/src/int_data_structures/b_int_set.cpp` |
| `pandaPI/engine/src/int_data_structures/bIntSet.h` | `pandaPI/engine/src/int_data_structures/b_int_set.h` |
| `pandaPI/engine/src/int_data_structures/bucketSet.cpp` | `pandaPI/engine/src/int_data_structures/bucket_set.cpp` |
| `pandaPI/engine/src/int_data_structures/bucketSet.h` | `pandaPI/engine/src/int_data_structures/bucket_set.h` |
| `pandaPI/engine/src/int_data_structures/delIterIntSet.cpp` | `pandaPI/engine/src/int_data_structures/del_iter_int_set.cpp` |
| `pandaPI/engine/src/int_data_structures/delIterIntSet.h` | `pandaPI/engine/src/int_data_structures/del_iter_int_set.h` |
| `pandaPI/engine/src/int_data_structures/noDelIntSet.cpp` | `pandaPI/engine/src/int_data_structures/no_del_int_set.cpp` |
| `pandaPI/engine/src/int_data_structures/noDelIntSet.h` | `pandaPI/engine/src/int_data_structures/no_del_int_set.h` |
| `pandaPI/engine/src/search/PriorityQueueSearch.cpp` | `pandaPI/engine/src/search/priority_queue_search.cpp` |
| `pandaPI/engine/src/search/PriorityQueueSearch.h` | `pandaPI/engine/src/search/priority_queue_search.h` |
| `pandaPI/engine/src/search/QueueFringe.cpp` | `pandaPI/engine/src/search/queue_fringe.cpp` |
| `pandaPI/engine/src/search/QueueFringe.h` | `pandaPI/engine/src/search/queue_fringe.h` |
| `pandaPI/engine/src/search/StackFringe.cpp` | `pandaPI/engine/src/search/stack_fringe.cpp` |
| `pandaPI/engine/src/search/StackFringe.h` | `pandaPI/engine/src/search/stack_fringe.h` |
| `pandaPI/engine/src/search/UnsortedFringe.cpp` | `pandaPI/engine/src/search/unsorted_fringe.cpp` |
| `pandaPI/engine/src/search/UnsortedFringe.h` | `pandaPI/engine/src/search/unsorted_fringe.h` |
| `pandaPI/engine/src/search/UnsrtFringeContainer.cpp` | `pandaPI/engine/src/search/unsrt_fringe_container.cpp` |
| `pandaPI/engine/src/search/UnsrtFringeContainer.h` | `pandaPI/engine/src/search/unsrt_fringe_container.h` |
| `pandaPI/engine/src/search/fringes/OneQueueWAStarFringe.cpp` | `pandaPI/engine/src/search/fringes/one_queue_wa_star_fringe.cpp` |
| `pandaPI/engine/src/search/fringes/OneQueueWAStarFringe.h` | `pandaPI/engine/src/search/fringes/one_queue_wa_star_fringe.h` |
| `pandaPI/engine/src/search/primeNumbers.h` | `pandaPI/engine/src/search/prime_numbers.h` |
| `pandaPI/engine/src/translation/htnToSAS.cpp` | `pandaPI/engine/src/translation/htn_to_sas.cpp` |
| `pandaPI/engine/src/translation/htnToSAS.h` | `pandaPI/engine/src/translation/htn_to_sas.h` |
| `pandaPI/engine/src/translation/translationController.cpp` | `pandaPI/engine/src/translation/translation_controller.cpp` |
| `pandaPI/engine/src/translation/translationController.h` | `pandaPI/engine/src/translation/translation_controller.h` |
