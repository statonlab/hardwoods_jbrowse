<?php

/**
 * @return array
 * @throws \Exception
 */
function hardwoods_jbrowse_instances_page() {
  $instances = hardwoods_jbrowse_get_instances();
  $settings = hardwoods_jbrowse_get_settings();

  $content = [];

  if (empty($instances)) {
    $content['empty'] = [
      '#type' => 'item',
      '#markup' => 'There are no instances at this time. Please use the link above to create a new one.',
    ];

    return $content;
  }

  $header = [
    'Organism',
    'Submitter',
    'Description',
    'Manage',
    'Delete'
  ];

  $rows = [];
  foreach ($instances as $instance) {
    $rows[] = [
      l(
        $instance->title,
        "{$settings['link']}",
        ['query' => hardwoods_jbrowse_build_http_query($instance)]
      ),
      $instance->user->name,
      $instance->description ?: 'Not provided',
      l('Manage Tracks', 'admin/hardwoods_jbrowse/instances/' . $instance->id),
      l('Delete Instance', 'admin/hardwoods_jbrowse/instances/'.$instance->id.'/delete')
    ];
  }

  $content['list'] = [
    '#type' => 'markup',
    '#markup' => theme(
      'table',
      [
        'rows' => $rows,
        'header' => $header,
      ]
    ),
  ];

  return $content;
}
