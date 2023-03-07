import http from 'k6/http';
import { check, sleep } from 'k6';
// Test configuration
export const options = {
  scenarios: {
    my_scenario1: {
      executor: 'constant-arrival-rate',
      duration: '10s', // total duration
      preAllocatedVUs: 1000, // to allocate runtime resources     preAll

      rate: 10, // number of constant iterations given `timeUnit`
      timeUnit: '1s',
    },
  },
};

// Simulated user behavior
export default function () {
  let responses = http.get('http://localhost:8080/3');
  // const responses = http.batch([
  //   ['GET', 'http://localhost:8080/1'],
  //   ['GET', 'http://localhost:8080/2'],
  //   ['GET', 'http://localhost:8080/3'],
  //   ['GET', 'http://localhost:8080/4'],
  //   ['GET', 'http://localhost:8080/5'],
  // ]);
  // Validate response status
  check(responses, {
    'status was 200': (r) => r.status == 200,
  });
}
