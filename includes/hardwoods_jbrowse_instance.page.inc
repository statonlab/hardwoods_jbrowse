<?php

function hardwoods_jbrowse_instance_page($instance_id) {
  $instance = hardwoods_jbrowse_get_instance($instance_id);

  if (empty($instance)) {
    drupal_not_found();
    return;
  }

  drupal_set_title("Manage $instance->title JBrowse");

  $content = '';
  $table = theme(
    'table',
    [
      'header' => ['Key', 'Value'],
      'rows' => [
        ['Instance Name', $instance->title],
        ['Created At', date('m/d/Y', $instance->created_at)],
        [
          'Organism',
          "{$instance->organism->genus} {$instance->organism->species}",
        ],
        ['Created By', "<b>{$instance->user->name}</b>"],
      ],
    ]
  );

  $content .= $table;
  return $content;
}
