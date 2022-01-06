using Microsoft.AspNetCore.JsonPatch;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;
using AiSearchAPI.Models;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using System.Reflection;

namespace AiSearchAPI.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class fifteenPuzzleController : ControllerBase
    {
        //public const string dllpath = @"D:\codingProjects\testAPI\aisearch_linux\bin\x64\Debug\libaisearch_linux.so";
        //public const string dllpath = @"libaisearch_linux.so";
        public const string dllpath = @"aisearch_dll.dll";
        [DllImport(dllpath)] 
        public static extern IntPtr create_puzzle(int size);

        [DllImport(dllpath)]
        public static extern int release_memory(IntPtr ptr);

        [DllImport(dllpath)]
        public static extern IntPtr solve_puzzle(int[] data, int size, ref int solved_puzzle_count);

        [DllImport(dllpath)]
        public static extern bool is_valid_puzzle(int[] data, int size);

        [HttpPost("SolvePuzzle")]
        public async Task<IActionResult> Solve_Puzzle([FromBody] object fields)
        {
            string jsonFields = fields.ToString();
            PuzzleModel model;
            try
            {
                model = JsonConvert.DeserializeObject<PuzzleModel>(jsonFields);
                int initial_row_length = model.data[0].Count;
                foreach (var row in model.data)
                {
                    if (row.Count != initial_row_length)
                        return BadRequest("ERROR: Invalid JSON Model Entry");
                }

                List<int> flattened_list = new List<int>();
                foreach (var row in model.data)
                {
                    foreach (var val in row)
                    {
                        flattened_list.Add(val);
                    }
                }

                //check if valid puzzle
                if (!is_valid_puzzle(flattened_list.ToArray(), model.data.Count))
                    throw new ArgumentException("Unsolvable Puzzle");

                //marshall string to integer array
                int solved_puzzle_count = 0;
                IntPtr puzzlePtr = await Task.Factory.StartNew(() =>
                {
                    return solve_puzzle(flattened_list.ToArray(), model.data.Count, ref solved_puzzle_count);
                });
                int arrayLength = solved_puzzle_count;
                IntPtr start = IntPtr.Add(puzzlePtr, 0);
                int[] result = new int[arrayLength];
                Marshal.Copy(start, result, 0, arrayLength);

                //convert to string
                PuzzlePath path_model = new PuzzlePath();
                for (int i = 1; i < result.Length; i++)
                {
                    path_model.path += Convert.ToChar(result[i]);
                }
                return Ok(path_model);
            }
            catch (Exception e)
            {
                switch (e)
                {
                    case NullReferenceException:
                        return BadRequest("ERROR: \"data\" moniker mispelled");
                    case ArgumentException:
                        return BadRequest("ERROR: " + e.Message);
                    default:
                        return BadRequest("Invalid JSON Model Entry");
                }
            }
        }

        [HttpGet("RandomPuzzle/{id}")]
        public async Task<IActionResult> Random_Puzzle(int id)
        {
            PuzzleModel model = new PuzzleModel();
            if (id <= 2)
                return BadRequest("ERROR: size must be greater than 2");
            try
            {
                model.data = await Task.Factory.StartNew(() =>
                {
                    return Create_Puzzle(id);
                });
                return Ok(model);
            }
            catch (Exception e)
            {
                switch (e)
                {
                    case NullReferenceException:
                        return BadRequest("ERROR: \"data\" moniker mispelled");
                    default:
                        return BadRequest("Invalid JSON Model Entry");
                }
            }
        }

        List<List<int>> Create_Puzzle(int size)
        {
            IntPtr puzzlePtr = create_puzzle(size);
            int arrayLength = size * size;
            IntPtr start = IntPtr.Add(puzzlePtr, 0);
            int[] result = new int[arrayLength];
            Marshal.Copy(start, result, 0, arrayLength);

            List<List<int>> return_puzzle = new List<List<int>>();
            List<int> current_row = new List<int>();
            for (int i = 0; i < result.Length; i++)
            {
                if (i % size == 0 && i != 0)
                {
                    return_puzzle.Add(current_row);
                    current_row = new List<int>();
                }
                current_row.Add(result[i]);
            }
            return_puzzle.Add(current_row);
            release_memory(start);
            return return_puzzle;
        }
    }

}
