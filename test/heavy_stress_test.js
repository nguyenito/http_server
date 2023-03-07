import http from 'k6/http';
import { check, sleep } from 'k6';
// Test configuration
export const options = {
  scenarios: {
    my_scenario1: {
      executor: 'constant-arrival-rate',
      duration: '5m', // total duration
      preAllocatedVUs: 1000, // to allocate runtime resources     preAll

      rate: 500, // number of constant iterations given `timeUnit`
      timeUnit: '1s',
    },
  },
};

// Simulated user behavior
export default function () {
  let res = http.get('http://localhost:8080/1');
  // Validate response status
  check(res, { 'status was 200': (r) => r.status == 200 });
}
