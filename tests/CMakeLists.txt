add_test(
    NAME bamtools_help
    COMMAND bamtools_cmd --help
)

add_test(
    NAME bamtools_stats
    COMMAND bamtools_cmd stats -in ${CMAKE_CURRENT_SOURCE_DIR}/data/sam_spec_example.bam
)
