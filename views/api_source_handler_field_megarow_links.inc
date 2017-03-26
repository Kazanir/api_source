<?php

/**
 * Megarow handler specific to API documentation entries, which are not
 * standard Drupal entities.
 */
class api_source_handler_field_megarow_links extends views_handler_field_megarow_links {
  public function render($values) {
    // Get the entity matching this row.
    $did = $values->api_documentation_api_members_did;

    // Create an array of links.
    $provided_links = explode("\n", $this->options['megarow']['links']);
    $provided_links = array_map('trim', $provided_links);
    $provided_links = array_filter($provided_links, 'strlen');

    $links = array();
    $tokens = $this->get_render_tokens(array());
    foreach ($provided_links as $link) {
      $link_parts = explode('|', $link);

      // Replace tokens if necessary in the url.
      $url = 'display_megarow/' . $did . '/' . $link_parts[1];
      $url = $this->render_altered(array('text' => $url), $tokens);

      // Do the same for the label.
      $label = $this->render_altered(array('text' => $link_parts[0]), $tokens);

      // Add the link for rendering.
      $links[] = $this->getLink($label, $url, array('class' => array('views-megarow-open')));
    }
    if (count($links) == 0) {
      $element = array();
    }
    else {
      $element = array(
        '#prefix' => $this->getElementPrefix($values),
        '#markup' => theme('links__ctools_dropbutton', array('links' => $links, 'attributes' => array('class' => array('links', 'api-source-button')))),
        '#suffix' => $this->getElementSuffix($values),
      );
      if ($GLOBALS['theme'] == 'bartik') {
        // Give us a decent default.
        $element['#attached'] = array(
          'css' => array(
            drupal_get_path('module', 'api_source') . '/api_source.css',
          ),
        );
      }
    }

    return $element;
  }
}

