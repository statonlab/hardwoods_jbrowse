<?php

function hardwoods_jbrowse_cmd_prepare_refseq($instance) {

}

function hardwoods_jbrowse_cmd_add_track($instance, $type, $file_path) {

}

/**
 * Safely execute a command.
 *
 * @param string $command The path to the command to execute.
 * @param array $args Arguments passed as flag => $argument or a list of
 *   arguments as [arg1, arg2, arg3]
 * @param array $output If the output argument is present, then the
 * specified array will be filled with every line of output from the
 * command. Trailing whitespace, such as \n, is not
 * included in this array. Note that if the array already contains some
 * elements, exec will append to the end of the array.
 * If you do not want the function to append elements, call
 * unset on the array before passing it to
 * exec.
 * @param int $return_var If the return_var argument is present
 * along with the output argument, then the
 * return status of the executed command will be written to this
 *
 * @return string
 */
function hardwoods_jbrowse_safe_exec(
  $command,
  array $args = [],
  &$output = NULL,
  &$return = NULL
) {
  $cmd = escapeshellcmd($command) . ' ';
  foreach ($args as $flag => $arg) {
    if (is_string($flag)) {
      $cmd .= escapeshellarg($flag) . ' ';
    }

    $cmd .= escapeshellarg($arg) . ' ';
  }

  print "Running the following command:\n";
  print $cmd."\n";

  return exec($cmd, $output, $return);
}
